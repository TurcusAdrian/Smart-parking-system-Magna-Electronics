#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>


void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void create_tabele(){

  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", "user",
          "BazaTest", 0, NULL, 0) == NULL)
  {
      finish_with_error(con);
  }

if (mysql_query(con, "DROP TABLE IF EXISTS cars")) {
      finish_with_error(con);
  }

  if (mysql_query(con, "CREATE TABLE cars(id INT PRIMARY KEY AUTO_INCREMENT, license_number VARCHAR(255))")) {
      finish_with_error(con);
  }

   mysql_close(con);

}

int main(){

    create_tabele();
}