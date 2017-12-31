

/**
 * \addtogroup      ESP_APP_HTTP_SERVER
 * \{
 *
 * \par             SSI (Server Side Includes) tags support
 *
 * SSI tags are supported on server to include user specific values as replacement of static content.
 *
 * Each tag must start with \ref HTTP_SSI_TAG_START tag and end with \ref HTTP_SSI_TAG_END and tag must not be longer than \ref HTTP_SSI_TAG_MAX_LEN.
 * White spaces are not allowed and "-" character is not allowed in tag name. Example of valid tag is <b>\<\!\-\-\#my_tag\-\-\></b> where name of tag is <b>my_tag</b>.
 *
 * The tag name is later sent to SSI callback function where user can send custom data as tag replacement.
 *
 * \par             CGI (Common Gateway Interface) support
 *
 * CGI support allows you to hook different functions from clients to server.
 *
 *  - CGI paths must be enabled before with callback functions when CGI is triggered
 *  - CGI path must finish with <b>.cgi</b> suffix
 *      - To allow CGI hook, request URI must be in format <b>/folder/subfolder/file.cgi?param1=value1&param2=value2&</b>
 *
 * \par             HTTP server example with CGI and SSI
 *
 * \code{c}
/*
 * CGI handler for CGI1,
 * Called when "/cgi1.cgi" is invoked in browser
 */
static char *
cgi1_callback(http_param_t* params, size_t params_len) {
    printf("CGI1 callback triggered\r\n");
    return "/index.shtml";
}

/*
 * CGI handler for CGI1,
 * Called when "/my_cgi.cgi" is invoked in browser
 */
static char *
cgi2_callback(http_param_t* params, size_t params_len) {
    printf("CGI2 callback triggered\r\n");
    return "/index.shtml";
}

/*
 * Create list of CGI handlers,
 * it consists of uri and callback pair
 */
const http_cgi_t
cgi_handlers[] = {
    { "/cgi1.cgi", cgi1_callback },         // Available on http://ip_addr/cgi1.cgi
    { "/my_cgi.cgi", cgi2_callback },       // Available on http://ip_addr/cgi2.cgi
};

/*
 * Single callback function for all SSI tags,
 * found in output templates
 *
 * User should use esp_http_server_write function,
 * to write data to output as replacement for the tag
 */
static size_t
http_ssi_cb(http_state_t* hs, const char* tag_name, size_t tag_len) {
    if (!strncmp("my_tag", tag_name, tag_len)) {
        esp_http_server_write(hs, "my_tag replacement string");
    }
    return 0;
}

/*
 * POST request started callback with
 * content length greater than 0
 */
static espr_t
http_post_start(http_state_t* hs, const char* uri, uint32_t content_len) {
    printf("POST started with content length: %d; on URI: %s\r\n", (int)content_len, uri);
    return espOK;
}

/*
 * POST request packet data received callback
 * It may be called multiple times, 
 * depends on request size
 */
static espr_t
http_post_data(http_state_t* hs, esp_pbuf_p pbuf) {
    printf("Data received: %d bytes\r\n", (int)esp_pbuf_length(pbuf, 1));
    return espOK;
}

/*
 * POST request finished callback
 */
static espr_t
http_post_end(http_state_t* hs) {
    printf("Post finished!\r\n");
    return espOK;
}

/*
 * Define server parameters structure,
 * later used by server application
 */
const http_init_t
http_init = {
    .post_start_fn = http_post_start,           /* Define POST start callback */
    .post_data_fn = http_post_data,             /* Define POST data callback */
    .post_end_fn = http_post_end,               /* Define POST end callback */
    .cgi = cgi_handlers,                        /* Define CGI handlers */
    .cgi_count = ESP_ARRAYSIZE(cgi_handlers),   //Set length of CGI handlers */
    .ssi_fn = http_ssi_cb,                      /* Set global SSI tags callback */
};

/* Later, somewhere in code: */
esp_http_server_init(&http_init, 80);           /* Enable server on port 80 */
\endcode
 *
 * \}
 */