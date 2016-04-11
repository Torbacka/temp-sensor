/*
 *  dht11.c:
 *	Simple test program to test the wiringPi functions
 *	DHT11 test
 */
#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#define MAXTIMINGS	85
#define DHTPIN		9
int dht11_dat[5] = { 0, 0, 0, 0, 0 };

void writeToFile(int h, int c);
//#include <my_global/my_global.h>
#include <mysql/mysql.h>

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}


void read_dht11_dat()
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	float	f; /* fahrenheit */

	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

	/* pull pin down for 18 milliseconds */
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 18 );
	/* then pull it up for 40 microseconds */
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 40 );
	/* prepare to read the pin */
	pinMode( DHTPIN, INPUT );

	/* detect change and read data */
	for ( i = 0; i < MAXTIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHTPIN );

		if ( counter == 255 )
			break;

		/* ignore first 3 transitions */
		if ( (i >= 4) && (i % 2 == 0) )
		{
			/* shove each bit into the storage bytes */
			dht11_dat[j / 8] <<= 1;
			if ( counter > 16 )
				dht11_dat[j / 8] |= 1;
			j++;
		}
	}

	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	printf("h: %d,    c: %d ", dht11_dat[0], dht11_dat[2]);
	if ( (j >= 40) &&
	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
	{
		f = dht11_dat[2] * 9. / 5. + 32;
		writeToFile(dht11_dat[0],dht11_dat[2]);
		 //"Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n",
		//			dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
	}else {
		printf("Data not good try read again");
		read_dht11_dat();
	}
}

void writeToFile(int h, int c)
{
	MYSQL *con = mysql_init(NULL);
	printf("H: %d,    C: %d", h, c);
  if (con == NULL)
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }
  if (mysql_real_connect(con, "localhost", "root", "torbackamysql",
          "senor_info", 0, NULL, 0) == NULL)
  {
      finish_with_error(con);
  }
    const char *sPS2 = "INSERT INTO Temperature (date, temperature, humidity) VALUES (NOW(),?,?)";
    int *temp;
    int *hum;
    MYSQL_STMT *stmt;
    MYSQL_BIND param[2];
    unsigned long l = 32;

    // Allocate a statement handle
    stmt = mysql_stmt_init(con);
    if(stmt == NULL) {
        printf("Unable to create new session: Could not init statement handle\n");
    }

    // Init
    memset(param, 0, sizeof(param));
    temp = (int *) malloc(sizeof(int));
    *temp = c;
    hum = (int *) malloc(sizeof(int));
    *hum = h;

    if(mysql_stmt_prepare(stmt, sPS2, strlen(sPS2)) != 0) {
        printf("Unable to create new session: Could not prepare statement\n");
    }

    // Initialise param structure
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = temp;
    param[0].is_unsigned = 0;
    param[0].is_null = 0;
    param[0].length = &l;

    param[1].buffer_type = MYSQL_TYPE_LONG;
    param[1].buffer = hum;
    param[1].is_unsigned = 0;
    param[1].is_null = 0;
    param[1].length = &l;

    // Bind param structure to statement
    if(mysql_stmt_bind_param(stmt, param) != 0) {
        printf("Unable to create new session: Could not bind parameters\n");
    }

    // Execute prepared statement
    if(mysql_stmt_execute(stmt) != 0) {
        printf("Unable to create new session: Could not execute statement\n");
    }

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
    mysql_close(con);
    free(temp);
    free(hum);
    //free(sPS2);

}
int main( void )
{
	//writeToFile(3,4);
	printf( "Raspberry Pi wiringPi DHT11 Temperature test program\n" );
	if ( wiringPiSetup() == -1 )
		exit( 1 );

	while ( 1 )
	{
		read_dht11_dat();
		delay(30*60*1000 ); /* wait 30 min to refresh*/ 
	}

	return(0);
}
