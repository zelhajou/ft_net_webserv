#include "Response.hpp"

Response::Response(): status(FIRST_LINE), _has_body(true), _new_session(false), _file_type("NONE"), _has_cookies(false) {
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



std::string	Response::generate_status_file(e_status status_code, ServerConfig *server, std::string addon) {
	std::map<int, std::string>::iterator	it = server->error_pages.find(status_code);
	if (it != server->error_pages.end() && access(it->second.c_str(), R_OK) == 0) return it->second;
	if (!addon.size())	addon = http_code_msg(status_code);
	std::string	status_file = CONFIG_PATH"/html_default_error_files/" + std::to_string(status_code) + clean_up_stuff(addon, "[\\]^`:;<>=?/ ", "_____________") + ".html";
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
		</style></head><body><h1>" << std::to_string(status_code) << "</h1><h2>" << addon << "</h2>\
		<a target='_blank' href=\"" << mdn_link << "\">developer.mozilla.org</a></body></html>";
	_file.close();
	this->_response_status = status_code;
	return	status_file;
}

static	bool	is_binary_data(std::string input) {
	int	cha;
	for (int i=0; i <input.size(); i++) {
		cha = static_cast<int>(input[i]);
		if (cha < 0 || cha > 126)
			return	true;
	}
	return	false;
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

static	e_status	print_Cstatus(e_status st) {
	int	status = (int)st;
	std::cout << KBGR;
	if (status == 200)		std::cout << KGRN;
	else if (status >= 500)	std::cout << KYEL;
	else if (status >= 400)	std::cout << KRED;
	else if (status >= 300)	std::cout << KCYN;
	std::cout << " " << st << " ";
	return	st;
}

static	std::string	generate_auto_index(std::string uri, ServerConfig *server) {
	std::string target =  CONFIG_PATH"/html_generated_files/"+ replace_characters(uri, "/", "#")+"S-"+std::to_string(get_dir_size(uri))+"Bytes"+".html";
	struct	stat	demo;
	if (stat(target.c_str(), &demo) != -1)	return target;
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


static	int	file_to_disk(std::string content, std::string path, std::string filename) {
	if (!filename.size())	filename = _generate_random_string(path, 15);
	std::fstream		file(path+"/"+filename, std::ios::out);
	if (!file.is_open())	return 0;
	file << content;
	file.close();
	return	1;
}

static	std::string	_conc_(std::string input, char c) {
	try {
		int	i(0);
		while (input[i] == c)
			input = input.substr(1);
		i = input.size() - 1;
		while (input[i] == c) {
			input = input.substr(0, i - 1);
			i = input.size() - 1;
		}
		return	input;
	}
	catch (std::exception &l)
	{
		return	input;
	}
}

static	std::string	_cgi_header(std::string input, std::string v_name, std::string v_name_2) {
		std::string		temp_c_t;
		int	pos = input.find(v_name);
		if (pos == std::string::npos) pos = input.find(v_name_2);
		if (pos != std::string::npos) {
			temp_c_t = input.substr(pos + v_name.size());
			pos = temp_c_t.find("\n");
			if (pos != std::string::npos) {
				temp_c_t = _conc_(temp_c_t.substr(0, pos), ' ');
				return	temp_c_t;
			}
		}
		return	"";
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
	std::string		out_file = CGI_COMM"/"+ _generate_random_string(this->_request->get_headers().host, 15) +".html";
	std::fstream	_file(out_file, std::ios::out);
	if (!_file.is_open())	return	this->generate_status_file(INTERNAL_SERVER_ERROR, server, http_code_msg(INTERNAL_SERVER_ERROR));
	std::string	input, to_stdin_input, output, _file_name;
	if (this->_request->get_first_line().method == "GET" && this->_request->_query_string.size()) {
		for (std::vector<std::pair<std::string, std::string> >::iterator it = this->_request->_query_string.begin();
			it != this->_request->_query_string.end(); ++it)
			input.append(it->first + "=" + it->second + "&");
	}
	else if (this->_request->get_first_line().method == "POST" && this->_request->get_headers().content_type.find("multipart/form-data") != std::string::npos) 
		input.append(this->_request->_request.body);
	else	input.append(this->_request->_request.raw_body);
	sock._enrg_env_var("CONTENT_LENGTH", std::to_string(input.size()));
	sock._enrg_env_var("REQUEST_METHOD", this->_request->get_first_line().method);
	sock._enrg_env_var("CONTENT_TYPE", this->_request->get_headers().content_type);
	sock._enrg_env_var("SERVER_NAME", server->server_name);
	sock._enrg_env_var("SERVER_PORT", server->listen_port);
	sock._enrg_env_var("SERVER_PROTOCOL", "HTTP/1.1");
	sock._enrg_env_var("HTTP_COOKIE", this->_request->get_headers().cookie);
	std::string	uri = this->_request->get_first_line().uri;
	if (!this->_request->_cgi_info.second.empty()) {
		sock._enrg_env_var("PATH_INFO", this->_request->_cgi_info.second);
		std::cout << "\t" << KCYN << "PATH_INFO: " << KNRM << this->_request->_cgi_info.second << std::endl;
		uri = uri.substr(0, uri.find(this->_request->_cgi_info.second));
	}	else	sock._enrg_env_var("PATH_INFO", "");
	sock._enrg_env_var("REQUEST_URI", uri);
	if (this->_request->get_first_line().method == "GET")	sock._enrg_env_var("QUERY_STRING", input);
	else	{ sock._enrg_env_var("QUERY_STRING", ""); to_stdin_input = input; }
	try {
		if (!to_stdin_input.empty()) {
			_file_name = CGI_COMM"/"+ _generate_random_string(uri, 20);
			std::fstream	_cgi_file(_file_name, std::ios::out|std::ios::binary);
			if (!_cgi_file.is_open())	throw std::runtime_error("can\'t setup comm. medium with cgi");
			_cgi_file << to_stdin_input;
			_cgi_file.close();
		}
		output = sock.execute_script(sock.format_env() + _M_DEL
			+ get_executer(this->_request->_cgi_info, uri) + _M_DEL
			+ uri + _M_DEL
			+ _file_name);
	} catch (std::exception &l) {
		std::cout << KRED << "\tResponse::process_cgi_exec(): just catched:" << l.what() << KNRM << std::endl;
		if (_file.is_open())	_file.close();
		return	this->generate_status_file(INTERNAL_SERVER_ERROR, server, http_code_msg(INTERNAL_SERVER_ERROR));
	}
	int		pos = output.find("webserv_cgi_status=");
	if (pos != std::string::npos) {
		int	cgi_status = std::atoi(output.substr(pos+19, pos+22).c_str());
		output = output.substr(pos+23);
		if (cgi_status != 200)	return	this->generate_status_file((e_status)cgi_status, server, output);
	}
	try {
		std::string	temp_c_t = _cgi_header(output, "Content-type:", "Content-Type:");
		if (!temp_c_t.empty() && sock.is_valid_mime(temp_c_t))
			this->_file_type = temp_c_t;
		temp_c_t = _cgi_header(output, "set-cookie:", "Set-cookie:");
		if (!temp_c_t.empty()) {
			this->_cgi_cookie = temp_c_t;
			this->_has_cookies = true;
		}
		//
		pos = output.find("\n\n");
		if (pos != std::string::npos)
			output = output.substr(pos + 2);
		else	throw	std::runtime_error("invalid cgi output formatting");
	} catch (std::exception &l) {
		std::cout << KRED << "\tResponse::process_cgi_exec(): just catched:" << l.what() << KNRM << std::endl;
		
		if (_file.is_open())	_file.close();
		return	this->generate_status_file(INTERNAL_SERVER_ERROR, server, http_code_msg(INTERNAL_SERVER_ERROR));
	}
	//
	_file << output;
	_file.close();
	return	out_file;
}

void	Response::_initiate_response(Request *req, Sockets &sock, ServerConfig *server) {
	this->_request = req;
	this->_response_status = this->_request->getStatus();
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
			std::cout << "\t" << KWHT << "--> CGI_REQUEST\n" << KNRM;
			if (!this->_request->_cgi_info.second.empty()) {
				int	pos = this->_request->_request.first_line.uri.rfind(this->_request->_cgi_info.second);
				if (pos != std::string::npos)
					this->_request->_request.first_line.uri = this->_request->_request.first_line.uri.substr(0, pos);
			}
			/*std::cout << "\t" << KCYN << "script_path: " << KNRM << this->_request->get_first_line().uri << std::endl;
			std::cout << "\t" << KCYN << "script_ext: " << KNRM << this->_request->_cgi_info.first << std::endl;*/
			if (access(this->_request->get_first_line().uri.c_str(), R_OK) < 0) {
				std::cout << KCYN << "\tscript_path_not_found\n" << KNRM;
				target_file = this->generate_status_file(NOT_FOUND, server, "CGI script NOT FOUND");
			}
			else {
				/*std::vector<std::pair<std::string, std::string> >::iterator	i = this->_request->_query_string.begin();
				for (;i != this->_request->_query_string.end();++i)
					std::cout << KCYN << "query_string["<< (i-this->_request->_query_string.begin()) <<"]:" << KNRM << i->first << "->" << i->second << std::endl;*/
				target_file = this->process_cgi_exec(sock, server);
			}
			std::cout << "\t" << KWHT << "<--\n" << KNRM;
		}
		else if (this->_request->get_first_line().method == "GET") target_file = this->_request->get_first_line().uri;
		else if (this->_request->get_first_line().method == "POST") {
			bool	post_status(true);	int	mini_post_status(1);
			if (this->_request->get_headers().content_type.find("multipart/form-data") != std::string::npos) {
				for (std::vector<t_post_body>::iterator i = this->_request->_post_body.begin(); i!=this->_request->_post_body.end(); ++i)
					if (i->filename.size() > 0) mini_post_status += file_to_disk(i->data, this->_request->get_first_line().uri, i->filename);


				/*if (mini_post_status != this->_request->_post_body.size() - 1) {
					post_status = false;
				}*/
			}
			else	post_status = file_to_disk(this->_request->_request.raw_body, this->_request->get_first_line().uri, "");
			target_file = this->generate_status_file(post_status ? this->_request->getStatus() : INTERNAL_SERVER_ERROR, server, "");
		}
		else if (this->_request->get_first_line().method == "DELETE")
			target_file = this->generate_status_file((std::remove(this->_request->get_first_line().uri.c_str()))
					? INTERNAL_SERVER_ERROR
					: this->_request->getStatus(), server, "");
	}
	else	target_file = this->generate_status_file(this->_request->getStatus(), server, "");
	
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
	if (server->default_session_managment)
		sock.check_session(*this);
}

e_parser_state	Response::get_status() { return this->status; }
void		Response::set_session_id( std::string id ) { this->_session_id = id; }

size_t	Response::form_headers(ServerConfig *server) {
	e_status	req_scode = this->_response_status;
	if (this->header.size())	this->header.clear();
	this->header = "HTTP/1.1 " + std::to_string(req_scode) + " " + http_code_msg(req_scode) + CRLF;
	this->header.append("Server: " + server->server_name + CRLF"Connection: " + this->_connection_type + CRLF);
	if (this->_request->_is_return && req_scode >= 300 && req_scode < 400) {
		this->header.append("Location: "+this->_request->_c_location->return_url.second+CRLF); }

	if (this->_has_cookies)	this->header.append("set-cookie: " + this->_cgi_cookie + " ;" + (this->_new_session ? "" : CRLF));
	if (this->_new_session)	this->header.append((this->_has_cookies ? std::string("") : std::string("set-cookie: ")) + "session="+ this->_session_id + "; "CRLF);
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
			this->status = ERROR;
			if (this->_has_body) {
				this->_sent[1] = 0;
				this->status = BODY;
			}
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
	if (this->status == DONE || this->status == ERROR) {
		this->_file.close();
		if (this->_request->_location_type == CGI) std::remove(target_file.c_str());
		std::cout << KBGR << " " << KNRM << " " << KBGR << "  " << this->_request->get_first_line().method
			<< " " << KNRM << " [" << KUND << this->_request->get_first_line().uri << KNRM << "] : "
			<< http_code_msg(print_Cstatus(this->_response_status)) << " " << KNRM << " : "
			<< KUND << this->_sent[1] << "B\n" << KNRM;
	}
}

