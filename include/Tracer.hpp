#include "Sockets.hpp"

class	Servers_tracer {
	public:
		int			_sessions;
		int			_open_connections;
		int			_served_files;
		int			_served_clients;
		size_t			_served_MBytes;
		int			_200;
		int			_4__;
		int			_5__;
		int			_3__;
		bool			_master_status;
		std::string		_html_head;
		std::string		_info_div;
		std::map<int, std::string>	_servers_divs;
		std::string		_html_tail;

		Servers_tracer(): _sessions(0), _open_connections(0), _served_files(0),
			_served_clients(0), _200(0), _3__(0), _4__(0), _5__(0) {
				this->_html_head = "<!--HEAD_START--><html><head><style>\
					* {  box-sizing: border-box;  padding: 0px;  margin: 0px;}body, div {  display: flex; \
					align-items: center;  justify-content: center;  flex-direction: column;}\
					body {  height: 100vh;  width: 100%;  justify-content: start;  background: \
						#3c3c3c;  row-gap: 2em;  padding: 5%;  color: #fff;  font-family: monospace;  \
					font-weight: thin;}h1 {  font-size: 18px;}h2 {  font-size: 15px;}#general_info {  width: 100%;  \
					flex-direction: row;  justify-content: end;  column-gap: 5em;}#general_info > div \
					{  height: 100%;  align-items: start;  justify-content: start;  row-gap: 1em;}#general_info > \
					div:nth-child(1) > h3:nth-child(3) {  display: flex;  flex-direction: row;  \
						    column-gap: 1em;  line-height: 1.2em;  justify-content: center;  \
						    align-items: center;}#general_info > div:nth-child(2) > h3 \
						{  line-height: 1.5em;}#general_info > div:nth-child(2) > h3 > span \
					{  text-decoration: underline;}#server {  /*max-height: 30em;  min-height: 10em;*/  \
					width: 100%;  padding: 1em;  row-gap: 1em;  border: 1px solid rgb(120,120,120);}#server > * \
					{  align-items: start;}#output {  width: 95%;  min-height: 20em;  background: #000;  \
					padding: 1em;  justify-content: start;  row-gap: 10px;  max-height: 80vh;  \
						overflow: auto;}#status {  width: 100%;  height: 2em;  flex-direction: row;  \
						align-items: center;  justify-content: start;  column-gap: 1em;  \
					padding: 0 1em;}#status > p, #general_info p {  height: 12px;  width: 12px;  \
					border-radius: 50%;}.on #status > p,#general_info #on {  background-color: green;}.off #status > p, \
					#general_info #off {  background-color: red;}#info {  width: 85%;  flex-direction: row; \
					justify-content: start;}#info > div {  height: 100%;}#info > div:nth-child(1) {  width: 20%;\
						align-items: start;  row-gap: 10px;}#info > div:nth-child(1) #field { \
						flex-direction: row;}#info > div:nth-child(2) {  width: 80%;}.off #output\
					{  display: none;}</style></head><body><!--HEAD_END-->";
				this->_html_tail = "<!--TAIL_START--></body></html><!--TAIL_END-->";
			}
		void	generate_info() {
			this->_info_div = "<!--INFO_START--><div id=\"general_info\"><div><h3>Sessions: "+std::to_string(_sessions)+"</h3>\
				         <h3>Open Connections: "+std::to_string(_open_connections)+"</h3><h3><p id=\""+(_master_status?"on":"off")+"\"></p>master process;</h3></div><div>\
					<h3>Served <span>"+std::to_string(_served_files)+"</span> Files ("+std::to_string(_served_MBytes)
					+" MB) to <br><span>"+std::to_string(_served_clients)+"</span> Clients</h3>\
 					<li>200: "+std::to_string(_200)+"</li><li>4**: "+std::to_string(_4__)+"</li><li>5**: "+std::to_string(_5__)
					+"</li><li>3**: "+std::to_string(_3__)+"</li></div></div><!--INFO_END-->";
		}
		void	add_server(int sock, std::string _server_name, std::string _port,
				std::string _host, bool status, std::map<std::string, LocationConfig> locations) {
			_servers_divs[ sock ] = "<!--SERVER_START--><div id=\"server\" class=\""+(status?std::string("on"):std::string("off"))+"\"><div id=\"status\"><p ></p><h1>"+_server_name+"</h1></div>\
					     <div id=\"info\"><div><div id=\"field\" class=\"host\"><h2>Host : </h2>\
				     	     <h2>"+_host+"</h2></div><div id=\"field\" class=\"port\"><h2>Port : </h2><h2>"+_port+"</h2>\
					     </div></div><div id=\"field\" class=\"locations\">";
			for (std::map<std::string, LocationConfig>::iterator i = locations.begin(); i!=locations.end(); ++i)
				_servers_divs[ sock ].append("<li>"+i->first+"  ->  "+i->second.root+"</li>");
			_servers_divs[ sock ].append("</div></div><div id=\"output\"><!--SERVER_OUTPUT_START--><!--SERVER_OUTPUT_END--></div></div><!--SERVER_END-->");
		}
		void	add_output(int sock, std::string input) {
			int pos = _servers_divs[ sock ].find("<!--SERVER_OUTPUT_END-->");
			std::string	temp_div = _servers_divs[ sock ].substr(0, pos);
			temp_div.append("<h3>> " + input + "</h3>");
			temp_div.append(_servers_divs[ sock ].substr(pos));
			_servers_divs[ sock ] = temp_div;
		}
		void	increment_status_code(int status) {
			if (status == 200)		this->_200 += 1;
			else if (status >= 500)	this->_5__ += 1;
			else if (status >= 400)	this->_4__ += 1;
			else if (status >= 300)	this->_3__ += 1;
		}
		std::string	generate_html() {
			this->generate_info();
			std::string		_html = this->_html_head;
			_html.append(this->_info_div);
			std::map<int, std::string>::iterator i = _servers_divs.begin();
			for (; i != _servers_divs.end(); ++i)
				_html.append(i->second);
			_html.append(this->_html_tail);
			return		_html;
		}
};
