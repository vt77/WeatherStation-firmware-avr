
#define LOG_LEVEL_NONE	 0
#define LOG_LEVEL_ERROR  1
#define LOG_LEVEL_INFO   2
#define LOG_LEVEL_DEBUG  3


namespace logger{

static uint8_t level = LOG_LEVEL_ERROR; 

void write_message(const char * message)
{
#ifdef LOGGER
	LOGGER.println(message);
#endif
}

void build_message(const char * level,const char *fmt, ...) {
    char buff[128];
    sprintf(buff,"[%s]",level);
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff+strlen(level)+2,128,fmt, args);
    va_end(args);
    write_message(buff);
}

void set_level(uint8_t l){level = l;}


#define debug(...) build_message("DEBUG",__VA_ARGS__)
#define info(...) build_message("INFO",__VA_ARGS__)
#define error(...) build_message("ERROR",__VA_ARGS__)

}