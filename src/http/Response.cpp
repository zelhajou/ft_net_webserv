#include "Response.hpp"

Response::Response(): status(FIRST_LINE), _has_body(true), _new_session(false), _file_type("NONE") {
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

static	std::string	generate_status_file(e_status status_code, ServerConfig *server, std::string addon) {
	std::map<int, std::string>::iterator	it = server->error_pages.find(status_code);
	if (it != server->error_pages.end() && access(it->second.c_str(), R_OK) == 0) return it->second;
	std::string	status_file = CONFIG_PATH"/html_default_error_files/" + std::to_string(status_code) + ".html";
	if (!access(status_file.c_str(), R_OK))	return status_file;
	std::fstream	_file(status_file, std::ios::out);
	if (!_file.is_open())	return "";
	std::string mdn_link = "https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/" + std::to_string(status_code);
	_file << "<html><head><title>"<<std::to_string(status_code)<<"</title><style>\
			* {margin:0px;padding:0px;box-sizing:border-box;}html{background-color:#000;}\
			body {height:100vh;width:100%;display:flex;align-items:center;row-gap:1em;\
				justify-content:center;flex-direction:column;font-family:monospace;}\
			h1 {color:#fff;font-size:15em;font-style:italic;text-shadow:5px 5px #7a7a7a;}\
			h2 {color:#7a7a7a;text-transform:uppercase;font-size:3em;}a{font-size:20px;}\
		</style></head><body><h1>"<<std::to_string(status_code)<<"</h1><h2>"<<(addon.size()?addon:http_code_msg(status_code))<<"</h2>\
		<a target='_blank' href=\""<<mdn_link<<"\">developer.mozilla.org</a></body></html>";
	_file.close();
	return	status_file;
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

static	std::string	_generate_random_string(std::string seed, int length) {
	std::string	output = seed;
	for (int i=0;i<length;i++) {
		std::string    r(1, static_cast<char>(std::rand() % (122 - 48) + 48 ));
		output.append( r );
	}
	return	clean_up_stuff(output, "[\\]^`:;<>=?/ ", "_____________");
}


static	int	file_to_disk(std::string content, std::string path, std::string filename) {
	if (!filename.size())	filename = _generate_random_string(path, 15);
	std::fstream		file(path+"/"+filename, std::ios::out);
	if (!file.is_open())	return 0;
	file << content;
	file.close();
	return	1;
}

static	std::string	get_executer(std::pair<std::string, std::string> cgi_info, std::string uri) {
	if (cgi_info.first.empty() || !access(uri.c_str(), X_OK))	return	uri;
	if (cgi_info.first == ".py")				return	PYTHON_PATH;
	if (cgi_info.first == ".php")				return	PHP_PATH;
	if (cgi_info.first == ".pl" || cgi_info.first == ".pm")	return	PERL_PATH;
	if (cgi_info.first == ".java" || cgi_info.first == ".jvs")		return	JAVA_PATH;
	if (cgi_info.first == ".js" || cgi_info.first == ".javascript")	return	JS_PATH;
	if (cgi_info.first == ".sh")					return	SHELL_PATH;
	return	uri;
}

std::string	Response::process_cgi_exec(Sockets &sock, ServerConfig *server) {
	std::string	out_file = CGI_OUTPUT"/"+ _generate_random_string(this->_request->get_headers().host, 15) +".html";
	std::fstream	_file(out_file, std::ios::out);
	if (!_file.is_open())	return	generate_status_file(INTERNAL_SERVER_ERROR, server, "");
	std::string	input, to_stdin_input, output;
	if (this->_request->_query_string.size()) {
		for (std::vector<std::pair<std::string, std::string> >::iterator i=this->_request->_query_string.begin();
			i!=this->_request->_query_string.end();++i)
			input.append(i->first+"="+i->second+"&");
	}
	if (this->_request->get_headers().content_type.find("multipart/form-data") != std::string::npos)
		for (std::vector<t_post_body>::iterator i=this->_request->_post_body.begin();i!=this->_request->_post_body.end();++i)
			input.append(i->name+"="+i->data+"&");
	else	input.append(this->_request->_request.raw_body);
	sock._enrg_env_var("CONTENT_LENGTH", std::to_string(input.size()));
	sock._enrg_env_var("REQUEST_METHOD", this->_request->get_first_line().method);
	sock._enrg_env_var("SERVER_NAME", server->server_name);
	sock._enrg_env_var("SERVER_PORT", server->listen_port);
	sock._enrg_env_var("SERVER_PROTOCOL", "HTTP/1.1");
	std::string	uri = this->_request->get_first_line().uri;
	if (!this->_request->_cgi_info.second.empty()) {
		sock._enrg_env_var("PATH_INFO", this->_request->_cgi_info.second);
		uri = uri.substr(0, uri.find(this->_request->_cgi_info.second));
	}
	if (this->_request->get_first_line().method == "GET")	sock._enrg_env_var("QUERY_STRING", input);
	else	to_stdin_input = input;
	try {
		output = sock.execute_script(sock.format_env() + _M_DEL
			+ get_executer(this->_request->_cgi_info, uri) + _M_DEL
			+ uri + _M_DEL
			+ to_stdin_input);
	} catch (std::exception &l) {
		std::cout << KRED << "Response::process_cgi_exec(): just catched:" << l.what() << KNRM << std::endl;
		if (_file.is_open())	_file.close();
		return	generate_status_file(INTERNAL_SERVER_ERROR, server, "");
	}
	int		pos = output.find("webserv_cgi_status=");
	if (pos != std::string::npos) {
		int	cgi_status = std::atoi(output.substr(pos+19, pos+22).c_str());
		std::cout << KGRN"cgi_status:"<< cgi_status<<std::endl<<KNRM;
		std::cout << KGRN"got:" << output.substr(pos+23) << KNRM << std::endl;
		if (cgi_status != 200)	return	generate_status_file((e_status)cgi_status, server, output.substr(pos+23));
	}
	try {
		pos = output.find("Content-Type: ");
		if (pos != std::string::npos) {
			std::string temp_c_t = output.substr(pos+14);
			output = temp_c_t;
			pos = temp_c_t.find("\n");
			if (pos != std::string::npos) {
				temp_c_t = temp_c_t.substr(0, pos);
				if (sock.is_valid_mime(temp_c_t))
					this->_file_type = temp_c_t;
				output = output.substr(pos);
			}
		}
	} catch (std::exception &l) {
		std::cout << KRED << "Response::process_cgi_exec(): just catched:" << l.what() << KNRM << std::endl;
		if (_file.is_open())	_file.close();
		return	generate_status_file(INTERNAL_SERVER_ERROR, server, "");
	}
	//
	_file << output;
	_file.close();
	return	out_file;
}

void	Response::_initiate_response(Request *req, Sockets &sock, ServerConfig *server) {
	this->_request = req;
	std::string	target_file;

	if (this->_request->getStatus() == OK && !this->_request->_is_return) {
		if (this->_request->_location_type == AUTOINDEX) {
			struct	stat	output;
			std::string	uri = this->_request->get_first_line().uri;
			std::memset(&output, 0, sizeof(output));
			if (stat(uri.c_str(), &output) == -1 || !S_ISDIR(output.st_mode)
				|| !(target_file = generate_auto_index(uri, server)).size()) {
				this->_request->setStatus(FORBIDDEN);
				this->_has_body = false;
			}
		}
		else if (this->_request->_location_type == CGI) {
			// CGI STUFF
			std::cout << "////////////////////////CGI_STUFF//////\n";
			std::cout << KCYN"uri:"KNRM << this->_request->get_first_line().uri << std::endl;
			std::cout << KCYN"method:"KNRM << this->_request->get_first_line().method << std::endl;
			std::vector<std::pair<std::string, std::string> >::iterator	i = this->_request->_query_string.begin();
			for (;i != this->_request->_query_string.end();++i)
				std::cout << KCYN"query_string:"KNRM << i->first << "->" << i->second << std::endl;
			//
			target_file = this->process_cgi_exec(sock, server);
			std::cout << "////////////////////////////////\n";
		}
		else if (this->_request->get_first_line().method == "GET") target_file = this->_request->get_first_line().uri;
		else if (this->_request->get_first_line().method == "POST") {
			bool	post_status(true);	int	mini_post_status(1);
			if (this->_request->get_headers().content_type.find("multipart/form-data") != std::string::npos) {
				for (std::vector<t_post_body>::iterator i = this->_request->_post_body.begin(); i!=this->_request->_post_body.end(); ++i)
					if (i->filename.size() > 0) mini_post_status += file_to_disk(i->data, this->_request->get_first_line().uri, i->filename);
				if (mini_post_status != this->_request->_post_body.size())	post_status = false;
			}
			else	post_status = file_to_disk(this->_request->_request.raw_body, this->_request->get_first_line().uri, "");
			target_file = generate_status_file(post_status ? this->_request->getStatus() : INTERNAL_SERVER_ERROR, server, "");
		}
		else if (this->_request->get_first_line().method == "DELETE")
			target_file = generate_status_file((std::remove(this->_request->get_first_line().uri.c_str()))
					? INTERNAL_SERVER_ERROR
					: this->_request->getStatus(), server, "");
	}
	else	target_file = generate_status_file(this->_request->getStatus(), server, "");
	
	if (this->_has_body) {
		this->_file.open(target_file, std::ios::in|std::ios::binary);
		if (!this->_file) {
			e_status	tempS = this->_request->getStatus();
			this->_request->setStatus(FORBIDDEN);
			if (tempS == OK) {
				this->_initiate_response(req, sock, server);
				return ;
			}
			else	this->_has_body = false;
		}
		else {
			this->_file_size = this->get_file_size();
			int	ppos = target_file.rfind(".");
			if (ppos == target_file.npos)	ppos = 0;
			if (this->_file_type == "NONE")
				this->_file_type = sock.get_mime_type(target_file.substr(ppos));
		}
	}
	this->_connection_type = (this->_request->get_headers().connection == "keep-alive") ? "keep-alive" : "close";
	sock.check_session(*this);
}

e_parser_state	Response::get_status() { return this->status; }
void		Response::set_session_id( std::string id ) { this->_session_id = id; }

size_t	Response::form_headers(ServerConfig *server) {
	e_status	req_scode = this->_request->getStatus();
	if (this->header.size())	this->header.clear();
	this->header = "HTTP/1.1 " + std::to_string(req_scode) + " " + http_code_msg(req_scode) + CRLF;
	this->header.append("Server: " + server->server_name + CRLF"Connection: " + this->_connection_type + CRLF);
	if (this->_request->_is_return && req_scode >= 300 && req_scode < 400) {
		this->header.append("Location: "+this->_request->_c_location->return_url.second+CRLF);
	}
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

