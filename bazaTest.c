#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

#include "populate.c"
#include "create_tabele.c"

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", "user","BazaTest", 0, NULL, 0) == NULL)
      finish_with_error(con);
  
  create_tabele();
  populate();
  
  

  

  mysql_close(con);
  exit(0);
}