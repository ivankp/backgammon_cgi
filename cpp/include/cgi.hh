#ifndef IVANP_CGI_HH
#define IVANP_CGI_HH

#include <map>
#include "sqlite.hh"
#include "cat.hh"

using str_map = std::map<std::string,std::string>;
using sql_map = std::map<std::string,ivanp::sqlite::value>;

std::string get_post_str();
str_map get_query();
str_map get_cookies();

sql_map sqlmap(ivanp::sqlite& db, const std::string& sql);

int cookie_login(ivanp::sqlite& db, const str_map& cookies); // return: userid

#endif
