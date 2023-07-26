#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "nlohmann/json.hpp"
#include "requests.hpp"
#include "buffer.hpp"

using namespace std;
using namespace nlohmann::literals;
using json = nlohmann::ordered_json;

class Client
{
private:
    enum comenzi
    {
        LOGIN = 1,
        REGISTER,
        ENTER_LIBRARY,
        GET_BOOKS,
        GET_BOOK,
        ADD_BOOK,
        DELETE_BOOK,
        LOGOUT,
        EXIT
    };
    map<string, Client::comenzi> dict = {
        {"login", LOGIN},
        {"register", REGISTER},
        {"enter_library", ENTER_LIBRARY},
        {"get_books", GET_BOOKS},
        {"get_book", GET_BOOK},
        {"add_book", ADD_BOOK},
        {"delete_book", DELETE_BOOK},
        {"logout", LOGOUT},
        {"exit", EXIT},
    };
    char URL[9][55] = {
        "https://youtu.be/Zq5OS5q4eTs",
        "/api/v1/tema/auth/login",
        "/api/v1/tema/auth/register",
        "/api/v1/tema/library/access",
        "/api/v1/tema/library/books",
        "/api/v1/tema/library/books/",
        "/api/v1/tema/library/books",
        "/api/v1/tema/library/books/",
        "/api/v1/tema/auth/logout"};
    char HOST[14] = "34.254.242.81";
    int sockfd;
    bool logat = false, access = false;
    string cookie, token;

public:
    void run()
    {
        string comanda;
        cout << "Introduceti comanda dorita. Comenzile sunt:\n"
             << "[1] register\n"
             << "[2] login\n"
             << "[3] enter_library\n"
             << "[4] get_books\n"
             << "[5] get_book\n"
             << "[6] add_book\n"
             << "[7] delete_book\n"
             << "[8] logout\n"
             << "[9] exit\n";
        while (true)
        {
            getline(cin, comanda);
            switch (dict[comanda])
            {
            case LOGIN:
                login();
                break;
            case REGISTER:
                registre();
                break;
            case ENTER_LIBRARY:
                enter_library();
                break;
            case GET_BOOKS:
                get_books();
                break;
            case GET_BOOK:
                get_book();
                break;
            case ADD_BOOK:
                add_book();
                break;
            case DELETE_BOOK:
                delete_book();
                break;
            case LOGOUT:
                logout();
                break;
            case EXIT:
                return;
                break;
            default:
                cout << "Comanda incorecta. Comenzile sunt:\n"
                     << "[1] register\n"
                     << "[2] login\n"
                     << "[3] enter_library\n"
                     << "[4] get_books\n"
                     << "[5] get_book\n"
                     << "[6] add_book\n"
                     << "[7] delete_book\n"
                     << "[8] logout\n"
                     << "[9] exit\n";
                break;
            }
        }
    }

private:
    void open_connection()
    {
        struct sockaddr_in serv_addr;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            throw runtime_error("ERROR opening socket");
        }
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(8080);
        inet_aton(HOST, &serv_addr.sin_addr);
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            throw runtime_error("ERROR connecting");
        }
    }
    void send_to_server(char *message)
    {
        int bytes, sent = 0;
        int total = strlen(message);

        do
        {
            bytes = write(sockfd, message + sent, total - sent);
            if (bytes < 0)
            {
                throw runtime_error("ERROR writing message to socket");
            }

            if (bytes == 0)
            {
                break;
            }

            sent += bytes;
        } while (sent < total);
    }
    string receive_from_server()
    {
        static constexpr char HEADER_TERMINATOR[] = "\r\n\r\n";
        static constexpr int HEADER_TERMINATOR_SIZE = (sizeof(HEADER_TERMINATOR) - 1);
        static constexpr char CONTENT_LENGTH[] = "Content-Length: ";
        static constexpr int CONTENT_LENGTH_SIZE = (sizeof(CONTENT_LENGTH) - 1);
        static constexpr int BUFLEN = 4096;
        char response[BUFLEN];
        Buffer buffer;
        int header_end = 0;
        int content_length = 0;

        do
        {
            int bytes = read(sockfd, response, BUFLEN);

            if (bytes < 0)
            {
                throw runtime_error("ERROR reading response from socket");
            }

            if (bytes == 0)
            {
                break;
            }

            buffer.buffer_add(response, (size_t)bytes);

            header_end = buffer.buffer_find(HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

            if (header_end >= 0)
            {
                header_end += HEADER_TERMINATOR_SIZE;

                int content_length_start = buffer.buffer_find_insensitive(CONTENT_LENGTH, CONTENT_LENGTH_SIZE);

                if (content_length_start < 0)
                {
                    continue;
                }

                content_length_start += CONTENT_LENGTH_SIZE;
                content_length = strtol(buffer.data + content_length_start, NULL, 10);
                break;
            }
        } while (1);
        size_t total = content_length + (size_t)header_end;

        while (buffer.size < total)
        {
            int bytes = read(sockfd, response, BUFLEN);

            if (bytes < 0)
            {
                throw runtime_error("ERROR reading response from socket");
            }

            if (bytes == 0)
            {
                break;
            }

            buffer.buffer_add(response, (size_t)bytes);
        }
        buffer.buffer_add("", 1);
        return string(buffer.data);
    }
    void login()
    {
        string user, pass;
        char *content = (char *)"application/json";
        do
        {
            cout << "username = ";
            getline(cin, user);
            if (user.empty() || user.find(' ') != string::npos)
            {
                cout << "Username invalid! Incercati din nou.\n";
            }
        } while (user.find(' ') != string::npos || user.empty());

        do
        {
            cout << "password = ";
            getline(cin, pass);
            if (pass.empty() || pass.find(' ') != string::npos)
            {
                cout << "Password invalid! Incercati din nou.\n";
            }
        } while (pass.empty() || pass.find(' ') != string::npos);

        if (logat)
        {
            cout << "User deja logat.\n";
            return;
        }
        open_connection();
        json j;
        j["username"] = user;
        j["password"] = pass;

        string json_string = j.dump(4, ' ', false, nlohmann::json::error_handler_t::replace);
        char *payload = (char *)json_string.c_str();
        size_t len = json_string.size();
        char *message = Requests::POST(HOST, URL[LOGIN], content, payload, len, NULL, (char *)token.c_str(), access);
        send_to_server(message);
        string response = receive_from_server();
        delete[] message;
        if (response.find("Too many requests") != string::npos)
        {
            cout << "Prea multe cereri catre server. Incercati mai tarziu.\n";
            close(sockfd);
            return;
        }
        if (response.find("error") != string::npos)
        {
            cout << "User/ pass gresit.\n";
            close(sockfd);
            return;
        }
        logat = true;
        string response_str = response;
        string::size_type beginning_cookie = response_str.find("connect.sid");
        if (beginning_cookie == string::npos)
        {
            close(sockfd);
            cout << "Eroare la luarea cookie-ului.\n";
            return;
        }
        string::size_type end_cookie = response_str.find(";", beginning_cookie);
        if (end_cookie == string::npos)
        {
            end_cookie = response_str.length();
        }
        cookie = response_str.substr(beginning_cookie, end_cookie - beginning_cookie);
        cout << "Autentificare cu succes.\n";
        close(sockfd);
    }
    void registre()
    {
        string user, pass;
        char *content = (char *)"application/json";
        do
        {
            cout << "username = ";
            getline(cin, user);
            if (user.empty() || user.find(' ') != string::npos)
            {
                cout << "Username invalid! Incercati din nou.\n";
            }
        } while (user.find(' ') != string::npos || user.empty());

        do
        {
            cout << "password = ";
            getline(cin, pass);
            if (pass.empty() || pass.find(' ') != string::npos)
            {
                cout << "Password invalid! Incercati din nou.\n";
            }
        } while (pass.empty() || pass.find(' ') != string::npos);

        if (logat)
        {
            cout << "User deja logat.\n";
            return;
        }
        open_connection();
        json j;
        j["username"] = user;
        j["password"] = pass;

        string json_string = j.dump(4, ' ', false, nlohmann::json::error_handler_t::replace);
        char *payload = (char *)json_string.c_str();
        size_t len = json_string.size();
        char *message = Requests::POST(HOST, URL[REGISTER], content, payload, len, NULL, (char *)token.c_str(), access);
        send_to_server(message);
        string response = receive_from_server();
        delete[] message;
        if (response.find("Too many requests") != string::npos)
        {
            cout << "Prea multe cereri catre server. Incercati mai tarziu.\n";
            close(sockfd);
            return;
        }
        if (response.find("error") != string::npos)
        {
            cout << "User deja inregistrat.\n";
            close(sockfd);
            return;
        }
        cout << "Inregistrare cu success.\n";
        close(sockfd);
    }
    void enter_library()
    {
        if (!logat)
        {
            cout << "User nu este logat.\n";
            return;
        }
        open_connection();
        char *briosa = NULL;
        if (!cookie.empty())
        {
            briosa = (char *)cookie.c_str();
        }
        char *message = Requests::GET(HOST, URL[ENTER_LIBRARY], NULL, briosa, (char *)token.c_str(), access);
        send_to_server(message);
        string raspuns = receive_from_server();
        delete[] message;
        if (raspuns.find("Too many requests") != string::npos)
        {
            cout << "Prea multe cereri catre server. Incercati mai tarziu.\n";
            close(sockfd);
            return;
        }
        if (raspuns.find("error") != string::npos)
        {
            cout << "Erorare in accesarea bibliotecii.\n";
            close(sockfd);
            return;
        }
        string::size_type begin_json = raspuns.find("{");
        if (begin_json == string::npos)
        {
            close(sockfd);
            cout << "Eroare in parsarea JSON-ului\n";
            return;
        }
        access = true;
        json j = json::parse(raspuns.substr(begin_json));
        token = j["token"];
        cout << "User are acum access la biblioteca.\n";
        close(sockfd);
    }
    void get_books()
    {
        if (!logat)
        {
            cout << "User nu este logat\n";
            return;
        }
        if (!access)
        {
            cout << "User nu are access la biblioteca.\n";
            return;
        }
        open_connection();
        char *briosa = NULL;
        if (!cookie.empty())
        {
            briosa = (char *)cookie.c_str();
        }
        char *message = Requests::GET(HOST, URL[GET_BOOKS], NULL, briosa, (char *)token.c_str(), access);
        send_to_server(message);
        string raspuns = receive_from_server();
        delete[] message;
        if (raspuns.find("Too many requests") != string::npos)
        {
            cout << "Prea multe cereri catre server. Incercati mai tarziu.\n";
            close(sockfd);
            return;
        }
        if (raspuns.find("error") != string::npos)
        {
            cout << "Erorare in accesarea bibliotecii.\n";
            close(sockfd);
            return;
        }
        string::size_type begin_json = raspuns.find("[");
        if (begin_json == string::npos)
        {
            close(sockfd);
            cout << "Eroare in parsarea JSON-ului\n";
            return;
        }
        json j = json::parse(raspuns.substr(begin_json));
        if (j.empty())
        {
            cout << "User nu are carti adaugate momentan.\n";
            close(sockfd);
            return;
        }
        cout << j.dump(4) << "\n";
        close(sockfd);
    }
    void get_book()
    {
        string id;
        do
        {
            cout << "id = ";
            getline(cin, id);
            if (!regex_match(id, regex("^[0-9]+$")))
            {
                cout << "ID trebuie sa fie un numar intreg.\n";
            }

        } while (!regex_match(id, regex("^[0-9]+$")));

        if (!logat)
        {
            cout << "User nu este logat\n";
            return;
        }
        if (!access)
        {
            cout << "User nu are access la biblioteca.\n";
            return;
        }
        open_connection();
        string ruta = string(URL[GET_BOOK]) + id;
        char *briosa = NULL;
        if (!cookie.empty())
        {
            briosa = (char *)cookie.c_str();
        }
        char *message = Requests::GET(HOST, (char *)ruta.c_str(), NULL, briosa, (char *)token.c_str(), access);
        send_to_server(message);
        string raspuns = receive_from_server();
        delete[] message;
        if (raspuns.find("Too many requests") != string::npos)
        {
            cout << "Prea multe cereri catre server. Incercati mai tarziu.\n";
            close(sockfd);
            return;
        }
        if (raspuns.find("error") != string::npos)
        {
            cout << "Erorare in accesarea cartii.\n";
            close(sockfd);
            return;
        }
        string::size_type begin_json = raspuns.find("{");
        if (begin_json == string::npos)
        {
            close(sockfd);
            cout << "Eroare in parsarea JSON-ului\n";
            return;
        }
        json j = json::parse(raspuns.substr(begin_json));
        cout << j.dump(4) << "\n";
        close(sockfd);
    }
    void add_book()
    {
        string title, author, genre, publisher, page_count;
        char *content = (char *)"application/json";
        do
        {
            cout << "title = ";
            getline(cin, title);
            if (title.empty())
            {
                cout << "Titlu invalid. Incercati din nou.\n";
            }
        } while (title.empty());
        do
        {
            cout << "author = ";
            getline(cin, author);
            if (author.empty() || !regex_match(author, regex("^[a-zA-Z. ]+$")))
            {
                cout << "Autor invalid. Incercati din nou.\n";
            }

        } while (author.empty() || !regex_match(author, regex("^[a-zA-Z. ]+$")));

        do
        {
            cout << "genre = ";
            getline(cin, genre);
            if (genre.empty() || !regex_match(genre, regex("^[a-zA-Z]+$")))
            {
                cout << "Genre invalid. Incercati din nou.\n";
            }

        } while (genre.empty() || !regex_match(genre, regex("^[a-zA-Z]+$")));

        do
        {
            cout << "publisher = ";
            getline(cin, publisher);
            if (publisher.empty() || !regex_match(publisher, regex("^[a-zA-Z. ]+$")))
            {
                cout << "Publisher invalid. Incercati din nou.\n";
            }

        } while (publisher.empty() || !regex_match(publisher, regex("^[a-zA-Z. ]+$")));

        do
        {
            cout << "page_count = ";
            getline(cin, page_count);
            if (!regex_match(page_count, regex("^[0-9]+$")))
            {
                cout << "Numarul de pagini trebuie sa fie un numar intreg.\n";
            }

        } while (!regex_match(page_count, regex("^[0-9]+$")));

        if (!logat)
        {
            cout << "User nu este logat\n";
            return;
        }
        if (!access)
        {
            cout << "User nu are access la biblioteca.\n";
            return;
        }
        open_connection();
        json j;
        j["title"] = title;
        j["author"] = author;
        j["genre"] = genre;
        j["publisher"] = publisher;
        j["page_count"] = stoi(page_count);
        string json_string = j.dump(4, ' ', false, nlohmann::json::error_handler_t::replace);
        char *payload = (char *)json_string.c_str();
        size_t len = json_string.size();
        char *briosa = NULL;
        if (!cookie.empty())
        {
            briosa = (char *)cookie.c_str();
        }
        char *message = Requests::POST(HOST, URL[ADD_BOOK], content, payload, len, briosa, (char *)token.c_str(), access);
        send_to_server(message);
        string response = receive_from_server();
        delete[] message;
        if (response.find("Too many requests") != string::npos)
        {
            cout << "Prea multe cereri catre server. Incercati mai tarziu.\n";
            close(sockfd);
            return;
        }
        if (response.find("error") != string::npos)
        {
            cout << "Eroare in adaugarea cartii.\n";
            close(sockfd);
            return;
        }
        cout << "Carte adaugata cu success.\n";
        close(sockfd);
    }
    void delete_book()
    {
        string id;
        do
        {
            cout << "id = ";
            getline(cin, id);
            if (!regex_match(id, regex("^[0-9]+$")))
            {
                cout << "ID trebuie sa fie un numar intreg.\n";
            }

        } while (!regex_match(id, regex("^[0-9]+$")));
        if (!logat)
        {
            cout << "User nu este logat\n";
            return;
        }
        if (!access)
        {
            cout << "User nu are access la biblioteca.\n";
            return;
        }
        open_connection();
        string ruta = string(URL[DELETE_BOOK]) + id;
        char *briosa = NULL;
        if (!cookie.empty())
        {
            briosa = (char *)cookie.c_str();
        }
        char *message = Requests::DELETE(HOST, (char *)ruta.c_str(), NULL, briosa, (char *)token.c_str(), access);
        send_to_server(message);
        string raspuns = receive_from_server();
        delete[] message;
        if (raspuns.find("Too many requests") != string::npos)
        {
            cout << "Prea multe cereri catre server. Incercati mai tarziu.\n";
            close(sockfd);
            return;
        }
        if (raspuns.find("error") != string::npos)
        {
            cout << "Erorare in stergerea cartii.\n";
            close(sockfd);
            return;
        }
        cout << "Carte stersa cu success.\n";
        close(sockfd);
    }
    void logout()
    {
        if (!logat)
        {
            cout << "User nu este logat\n";
            return;
        }
        char *briosa = NULL;
        if (!cookie.empty())
        {
            briosa = (char *)cookie.c_str();
        }
        open_connection();
        char *message = Requests::GET(HOST, URL[LOGOUT], NULL, briosa, (char *)token.c_str(), access);
        send_to_server(message);
        string raspuns = receive_from_server();
        delete[] message;
        if (raspuns.find("Too many requests") != string::npos)
        {
            cout << "Prea multe cereri catre server. Incercati mai tarziu.\n";
            close(sockfd);
            return;
        }
        if (raspuns.find("error") != string::npos)
        {
            cout << "Erorare in deconectarea clientului.\n";
            close(sockfd);
            return;
        }
        logat = access = false;
        cookie = token = "";
        cout << "Client deconectat cu success.\n";
        close(sockfd);
    }
};

int main()
{
    auto client = make_unique<Client>();
    client->run();
    return 0;
}