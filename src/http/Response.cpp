#include "Response.hpp"

Response::Response(): status(FIRST_LINE), _has_body(true), _new_session(false) {
	this->_sent.push_back(0);
	this->_sent.push_back(0);
}

Response::Response(const Response &R) { *this = R; }
Response	&Response::operator = (const Response &R) { return *this; }
Response::~Response() {}

size_t	Response::get_file_size() {
	std::streampos	fpos = this->_file.tellg();
	this->_file.seekg(0, std::ios::end);
	size_t	size = this->_file.tellg()-fpos;
	this->_file.seekg(0, std::ios::beg);
	return (size);
}

static	std::string	generate_status_file(e_status status_code, ServerConfig *server) {
	std::map<int, std::string>::iterator	it = server->error_pages.find(status_code);
	if (it != server->error_pages.end() && stat(it->second.c_str(), NULL) != -1)
		return	it->second;
	return	CONFIG_PATH"/html_default_error_files/" + std::to_string(status_code) + ".html";
}

static	std::string	replace_characters(std::string input, std::string from, std::string to) {
	int	i;
	while ((i = input.find(from)) != input.npos)
		input.replace(i, from.size(), to);
	return	input;
}

static	size_t		get_dir_size(std::string path) {
	struct	stat	output;
	if (stat(path.c_str(), &output) == -1)	return 0;
	return	output.st_size;
}

static	std::string	generate_auto_index(std::string uri, ServerConfig *server) {
	std::string target =  CONFIG_PATH"/html_generated_files/"+ replace_characters(uri, "/", "#")+"S-"+std::to_string(get_dir_size(uri))+"Bytes"+".html";
	std::cout << KRED << "directory listing requested:\n" << KNRM << KCYN << target << KNRM << std::endl;
	struct	stat	demo;
	if (stat(target.c_str(), &demo) != -1) {
		std::cout << KRED << "already generated html file\n" << KNRM;
		return	target;
	}
	std::fstream	output(target, std::ios::out);
	if (!output)	return	"";
	output << "<html><head><title> | Directory Listing</title>\
		<style>*{margin:0px;padding:0px;box-sizing:border-box;}html{background-color:#000;}body{\
		height:100vh;width:100%;display:flex;align-items:center;justify-content:center;flex-direction:column;font-family:monospace;color:#f7f7f7;}\
		#container{height:100%;width:90%;display:flex;border:1px solid #404040;flex-direction:column;justify-content:space-around;align-items:start;\
		max-height:90%;overflow:auto;scroll-behavior:smooth;padding:4em;}h1{color:#c8c8c8;font-size:1.7em;}\
		#container > div{height:90%;width:100%;display:flex;flex-direction:column;padding:2em 5em;align-items:start;background-color:#333;}\
		a{font-size:1.3em;height:2em;width:100%;display:flex;flex-direction:row;justify-content:space-between;align-items:center;\
		transition-duration:.3s;font-family:monospace;text-decoration:none;color:inherit;padding:0em 10px;}p{font-size:0.9em;\
		color:inherit;}hr {background-color:#4a4a4a;width:99%;height:1px;border:0px;align-self:center;}\
		a:hover{background-color:#f7f7f7;color:#333;}</style>\
		<script>function c_redir(loc){if (window.location.href.endsWith(\"#\")) window.location.href = window.location.href.slice(0, -1);\
		if (!window.location.href.endsWith(\"/\")) loc = \"/\" + loc; window.location.href += loc;}</script>\
		</head><body><div id=\"container\">\
		<h1>" << uri << " :</h1><div>";
	////////////////
	DIR			*dir = opendir(uri.c_str());
	struct	dirent		*direct;
	struct	stat		file_status;
	std::string		new_uri;
	while ((direct = readdir(dir))) {
		new_uri = uri + "/";
		new_uri.append(direct->d_name);
		output << "<a href=\"";
		if (!std::strcmp(direct->d_name, ".") || !std::strcmp(direct->d_name, ".."))
			output << direct->d_name<< "\">" << direct->d_name;
		else	output << "javascript:;\" onclick=\"c_redir('" << direct->d_name << "')\">" << direct->d_name;
		std::memset(&file_status, 0, sizeof(file_status));
		if (stat(new_uri.c_str(), &file_status) == -1) {
			output << "<p style=\"color:red;\">size_retrieval_failed</p></a><hr/>";
			continue;
		}
		output <<"<p>"<< std::to_string(file_status.st_size)<<" B</p></a><hr/>";
	}
	///////////////
	output <<	"</div></div></body></html>\n";
	output.close();
	return	target;
}

void	Response::_initiate_response(Request *req, Sockets &sock, ServerConfig *server) {
	this->_request = req;
	std::string	target_file;

	if (req->get_location_type() == AUTOINDEX) {
		struct	stat	output;
		std::string	uri = req->get_first_line().uri;
		std::memset(&output, 0, sizeof(output));
		if (stat(uri.c_str(), &output) == -1 || !S_ISDIR(output.st_mode)
			|| !(target_file = generate_auto_index(uri, server)).size()) {
			this->_request->setStatus(FORBIDDEN);
			this->_has_body = false;
		}
	}
	else if (req->getStatus() == OK && req->get_first_line().method == "GET")
		target_file = req->get_first_line().uri;
	else	
		target_file = generate_status_file(req->getStatus(), server);
	if (this->_has_body) {
		this->_file.open( target_file, std::ios::in|std::ios::binary);
		if (!this->_file) {
			this->_request->setStatus(FORBIDDEN);
			this->_has_body = false;
		} else {
			this->_file_size = this->get_file_size();
			int	ppos = target_file.rfind(".");
			if (ppos == target_file.npos) ppos = 0;
			this->_file_type = sock.get_mime_type(target_file.substr(ppos));
		}

	}
	this->_connection_type = req->get_headers().connection.find("keep-alive") != std::string::npos ? "keep-alive": "close";
	sock.check_session(*this);
}

e_parser_state	Response::get_status() { return this->status; }
void		Response::set_session_id( std::string id ) { this->_session_id = id; }

static	std::string	http_code_msg(e_status code)
{
	switch (code) {
		case OK:				return "OK";
		case BAD_REQUEST:			return "Bad Request";
		case NOT_FOUND:			return "Not Found";
		case FORBIDDEN:			return "Forbidden";
		case INTERNAL_SERVER_ERROR:		return "Internal Server Error";
		case NOT_IMPLEMENTED:		return "Not Implemented";
		case REDIRECT:			return "Redirect";
		case NOT_MODIFIED:			return "Not Modified";
		case TOO_MANY_REQUESTS:		return "Too Many Requests";
		case REQUEST_ENTITY_TOO_LARGE:	return "Request Entity Too Large";
		case REQUEST_HEADER_FIELDS_TOO_LARGE:	return "Request Header Fields Too Large";
		case HTTP_VERSION_NOT_SUPPORTED:	return "Http Version NOT Supported";
		case URI_TOO_LONG:			return "Url Too Long";
		case LENGTH_REQUIRED:		return "Length Required";
		case REQUEST_TIMEOUT:		return "Request Timeout";
		default:				return " ";
	}
}

size_t	Response::form_headers(ServerConfig *server) {
	e_status	req_scode = this->_request->getStatus();
	if (this->header.size())	this->header.clear();
	this->header = "HTTP/1.1 " + std::to_string(req_scode) + " " + http_code_msg(req_scode) + CRLF;
	this->header.append("Server: " + server->server_name + CRLF"Connection: " + this->_connection_type + CRLF);
	//if (req_scode == REDIRECT)	this->header.append("Location: "+/**/+CRLF);
	if (this->_new_session)	this->header.append("set-cookie: session="+ this->_session_id + "; "CRLF);
	//
	if (this->_has_body) {
		this->header.append("Content-type: " + this->_file_type + CRLF);
		this->header.append("Content-Length: " + std::to_string(this->_file_size) + CRLF);
	}
	//
	this->header.append(CRLF);
	return this->header.size();
}

void	Response::sendResponse(int sock_fd, ServerConfig *server) {
	int		sent_res;
	if (this->status == FIRST_LINE)
	{
		this->_sent[1] = this->form_headers(server);
		this->status = HEADERS;
		return ;
	}
	if (this->status == HEADERS)
	{

		sent_res = send(sock_fd, this->header.c_str(), this->header.size(), 0);
		if (sent_res < 0)	this->_sent[0] = this->_sent[1];
		else		this->_sent[0] += sent_res;

		if (this->_sent[0] >= this->_sent[1]) {
			if (this->_has_body) {
				this->_sent[1] = 0;
				this->status = BODY;
			}
			else	this->status = DONE;
		}
		else	this->header = this->header.substr(sent_res);
	}
	else if (this->status == BODY)
	{
		char	buffer[FILE_READ_BUFFER_SIZE];

		std::memset(buffer, 0, sizeof(buffer));
		if (!this->_sent[1]) {
			this->_sent[1] = this->_file_size;
			this->_sent[0] = 0;
		}
		this->_file.read(buffer, FILE_READ_BUFFER_SIZE);
		sent_res = this->_file.gcount();
		for (int bytes_sent=0, temp_perc=0; bytes_sent < sent_res;) {
			temp_perc = send(sock_fd, buffer, sent_res, 0);
			if (temp_perc < 0) {
				this->_sent[0] = this->_sent[1];
				break ;
			}
			bytes_sent += temp_perc;
		}
		this->_sent[0] += sent_res;
		if (this->_sent[0] >= this->_sent[1])	this->status = DONE;
	}
	std::cout << "sent " << sent_res <<" Bytes "<< "to "
		<< KCYN << sock_fd << KNRM << std::endl;
	if (this->status == DONE) {
		this->_file.close();
		std::cout << "done sending " << KCYN << this->_sent[1] << KNRM << " Bytes to " << KCYN << sock_fd
			<< KNRM << " in session: " << KCYN << this->_session_id << KNRM << std::endl;
	}
}

