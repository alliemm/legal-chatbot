#include <iostream>
#include <string>
#include "crow.h"
#include "crow/middlewares/session.h"
#include "libenvpp/env.hpp"
#include <pqxx/pqxx>
#include <sodium.h>

std::string getConnectionString();
std::unique_ptr<pqxx::connection> connectToDatabase(const std::string& postgresConnectionString);
bool checkPassword(const std::string& email, const std::string& password, const std::string& connectionString);

int main() {
    //initialize sodium library
    if (sodium_init() < 0)
    {
        return 1;
    }
    //get connection string once
    std::string connectionString = getConnectionString();

    //setup sessions
    //using file store so that session data is stored in json files
    using Session = crow::SessionMiddleware<crow::FileStore>;
    crow::FileStore sessionStore("./sessionData");
    //create app
    crow::App<crow::CookieParser, Session> app{
        Session{sessionStore}
    };

    app.loglevel(crow::LogLevel::Debug);

    CROW_ROUTE(app, "/")([](){
        return "This is just a test!";
    });

    //signup
    CROW_ROUTE(app, "/signup").methods("POST"_method)
    ([&app, connectionString](const crow::request& req){
        auto data = crow::json::load(req.body);
        //if there is no data or if the data is missing the email or password field return error
        if (!data || !data.has("password") || !data.has("email"))
        {
            return crow::response(400, "Missing signup information");
        }
        //get the email, password, and username values
        std::string password= data["password"].s();
        std::string email= data["email"].s();
        //check if the email is already taken and return error/message saying that username is taken
        auto conn = connectToDatabase(connectionString);
        if (!conn)
        {
            return crow::response(500, "Failed to connect to database");
        }
        //start a transaction
        pqxx::work transaction(*conn);
        //run query to check if email exists in database
        pqxx::result res = transaction.exec("SELECT * FROM users WHERE email = $1",pqxx::params{email});
        //if the email isn't in res then return false
        if (res.empty())
        {
            //hash password
            char hashed[crypto_pwhash_STRBYTES];
            int hashRes = crypto_pwhash_str(hashed, password.c_str(), password.length(),crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE);
            if (hashRes !=0)
            {
                std::cerr << "Failed to hash password" << std::endl;
                return crow::response(500, "Failed to hash password");
            }
            //convert hashed password to a string
            std::string finalHash = std::string(hashed);
            //add new user information into database
            transaction.exec("INSERT INTO users VALUES ($1, $2)",pqxx::params{email, finalHash});
            //end transaction
            transaction.commit();
            //store the current user as authenticated
            auto& session = app.get_context<Session>(req);
            session.set("user", std::string(email));
            return crow::response(201, "Created new account");
        }else
        {
            //since it failed abort the transaction
            transaction.abort();
            return crow::response(400, "This email address is already taken");
        }


    });

    //login
    CROW_ROUTE(app, "/login").methods("POST"_method)
    ([&app, connectionString](const crow::request& req){
        auto data = crow::json::load(req.body);
        //if there is no data or if the data is missing the username or password or email field return error
        if (!data || !data.has("email") || !data.has("password"))
        {
            return crow::response(400, "Missing login information");
        }
        std::string email= data["email"].s();
        std::string password= data["password"].s();
        //verify that the user exists and used the correct password
        if (checkPassword(email,password, connectionString))
        {
            auto& session = app.get_context<Session>(req);
            session.set("user", std::string(email));

            return crow::response(202, "User logged in");
        }else
        {
            return crow::response(400, "Incorrect login information");
        }

    });
    //logout
    CROW_ROUTE(app, "/logout")([&app](const crow::request& req){
        auto& session = app.get_context<Session>(req);
        //return error since user isn't logged
        if (session.get("user", "").empty())
        {
            return crow::response(400, "Already logged out");
        }
        std::cout << "Logging out"  << std::endl;
        //remove user from session
        session.remove("user");
        //return success
        return crow::response(200, "User logged out");
    });
    //delete account
    CROW_ROUTE(app, "/deactivate").methods("DELETE"_method)
   ([&app, connectionString](const crow::request& req){
       auto& session = app.get_context<Session>(req);
       std::string email = session.get("user", "");
       //if the user isn't logged in then respond with error
       if (email.empty())
       {
           return crow::response(401, "Unauthorized");
       }
       //else delete the user from all tables in the database
       //connect to database
       auto conn = connectToDatabase(connectionString);
       if (!conn)
       {
           return crow::response(500, "Failed to connect to database");
       }
       //start a transaction
       pqxx::work transaction(*conn);
       //run query to delete from all databases
       pqxx::result res = transaction.exec("DELETE FROM users WHERE email = $1",pqxx::params{email});
       res = transaction.exec("DELETE FROM surveys WHERE email = $1",pqxx::params{email});
       res = transaction.exec("DELETE FROM documents WHERE email = $1",pqxx::params{email});
       //commit the transaction
       transaction.commit();
       return crow::response(200, "Account Deleted");
   });

    CROW_ROUTE(app, "/survey").methods("POST"_method)
  ([&app, connectionString](const crow::request& req){
      //ensure that user is logged in
      auto& session = app.get_context<Session>(req);
      std::string email = session.get("user", "");
      //if the user isn't logged in then respond with error
      if (email.empty())
      {
          return crow::response(401, "Unauthorized");
      }
      //get the body of the request
      auto data = crow::json::load(req.body);
      //if there is no data or if the data is missing any of the necessary fields return an error
      if (!data || !data.has("userType") || !data.has("workedWithLawyer") || !data.has("howOften") || !data.has("typesOfDocuments")
          || !data.has("biggestConcerns") || !data.has("jargonUnderstanding") || !data.has("outcome"))
      {
          return crow::response(400, "Missing survey information");
      }
      //get survey answers from request
      std::string userType= data["userType"].s();
      std::string workedWithLawyer = data["workedWithLawyer"].s();
      std::string howOften = data["howOften"].s();
      std::string typesOfDocuments = data["typesOfDocuments"].s();
      std::string biggestConcerns = data["biggestConcerns"].s();
      std::string jargonUnderstanding = data["jargonUnderstanding"].s();
      std::string outcome = data["outcome"].s();
      //connect to database
      auto conn = connectToDatabase(connectionString);
      if (!conn)
      {
          return crow::response(500, "Failed to connect to database");
      }
      //start a transaction
      pqxx::work transaction(*conn);
      //run query to add user survey answers to database
      pqxx::result res = transaction.exec("INSERT INTO surveys VALUES ($1, $2, $3, $4, $5, $6, $7, $8)",
          pqxx::params{email, userType, workedWithLawyer, howOften, typesOfDocuments, biggestConcerns, jargonUnderstanding, outcome});
      //commit the transaction
      transaction.commit();
      return crow::response(200, "Saved survey results");
  });


    app.port(18080).multithreaded().run();
    //app.port(18080).run();

}

std::string getConnectionString()
{
    auto envPre = env::prefix("LEGALCHATBOT");
    const auto connectVar_id  = envPre.register_required_variable<std::string>("CONSTRING");
    auto validPre = envPre.parse_and_validate();
    return validPre.get(connectVar_id);
}

std::unique_ptr<pqxx::connection> connectToDatabase(const std::string& postgresConnectionString)
{
    //try to connect to database
    try
    {
        auto conn = std::make_unique<pqxx::connection>(postgresConnectionString);
        if (conn->is_open())
        {
            std::cout << "Connection to database established!" << std::endl;
            return conn;
        }
        return nullptr;
    }
    catch (std::exception& e)
    {
        //if it failed to connect print out the error message and return a nullptr
        std::cerr << e.what() << std::endl;
        return nullptr;
    }
}

bool checkPassword(const std::string& email, const std::string& password, const std::string& connectionString)
{
    try
    {
        //attempt to connect to the database
        auto conn = connectToDatabase(connectionString);
        if (!conn)
        {
            return false;
        }
        //start a transaction
        pqxx::nontransaction nonTransaction(*conn);
        //compare password in the database and the password the user entered
        std::cout << "Checking password" << std::endl;
        pqxx::result res = nonTransaction.exec("SELECT password FROM users WHERE email = $1",pqxx::params{email});
        if (res.empty())
        {
            return false;
        }else
        {
            std::string hashedPassword = res[0]["password"].as<std::string>();
            if (crypto_pwhash_str_verify(hashedPassword.c_str(), password.c_str(), password.length()) != 0) {
                //the password is incorrect so return false
                return false;
            }else
                return true;
        }
    }
    catch (std::exception& e)
    {
        //if we have an error printout a message and return false
        std::cerr << e.what() << std::endl;
        return false;
    }
}