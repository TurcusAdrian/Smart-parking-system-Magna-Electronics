#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

// gcc -o p populate.c -I/usr/include/mysql -lmysqlclient

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void populate(){
 MYSQL *con = mysql_init(NULL);

  if (con == NULL)
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", "user",  "BazaTest", 0, NULL, 0) == NULL)
      finish_with_error(con);
  


  if (mysql_query(con, "INSERT INTO cars VALUES(10, 'BN18CTL')")) {
      finish_with_error(con);
  }
  

   mysql_close(con);

}

int main(){
    populate();
}