import os
import cgi
import cgitb
import sys
import pickle
from http import cookies as Cookies
import time
import hashlib
import shutil

cgitb.enable()

class Session:
	def __init__(self, name, sid):
		self.name = name
		if sid:
			self.sid = sid
		else:
			self.sid = hashlib.sha1(str(time.time()).encode("utf-8")).hexdigest()
		current_dir = os.path.dirname(os.path.abspath(__file__))
		if not os.path.exists(current_dir + '/sessions'):
			os.makedirs(current_dir + '/sessions')
		with open(current_dir + '/sessions/session_' + self.sid, 'wb') as f:
			print(f"Session file created: session_{self.sid}", file=sys.stderr)
			pickle.dump(self, f)

	def getSid(self):
		return self.sid

class UserDataBase:
	def __init__(self):
		self.user_pass = {}
		self.user_firstname = {}
		self.logged_in = {}
		self.user_files = {}

	def addUser(self, username, password, firstname):
		self.user_pass[username] = password
		self.user_firstname[username] = firstname
		self.logged_in[username] = False
		self.user_files[username] = []
		current_dir = os.path.dirname(os.path.abspath(__file__))
		if not os.path.exists(current_dir + '/database'):
			os.makedirs(current_dir + '/database')
		with open(current_dir + '/database/user_database', 'wb') as f:
			pickle.dump(self, f)

def authUser(name, password):
	current_dir = os.path.dirname(os.path.abspath(__file__))
	if not os.path.exists(current_dir + '/database/user_database'):
		return None
	with open(current_dir + '/database/user_database', 'rb') as f:
		database = pickle.load(f)
		if name in database.user_pass and database.user_pass[name] == password:
			if 'HTTP_COOKIE' in os.environ:
				if "SID" in cook:
					if os.path.exists(current_dir + '/sessions/session_' + cook["SID"].value):
						with open(current_dir + '/sessions/session_' + cook["SID"].value, 'rb') as f:
							session = pickle.load(f)
							if session.name == name:
								return session
					session = Session(name, cook["SID"].value)
					return session
		else:
			return None

def isLoggedIN(name, sid):
	current_dir = os.path.dirname(os.path.abspath(__file__))
	if not os.path.exists(current_dir + '/database/user_database'):
		return None

	with open(current_dir + '/database/user_database', 'rb') as f:
		database = pickle.load(f)
		if name in database.user_pass and database.logged_in[name] == True:
			current_dir = os.path.dirname(os.path.abspath(__file__))
			if not os.path.exists(current_dir + '/sessions'):
				os.makedirs(current_dir + '/sessions')
			with open(current_dir + '/sessions/session_' + sid, 'rb') as f:
				session = pickle.load(f)
				if session.name == name:
					return True

def setLoggedIN(name, status):
	current_dir = os.path.dirname(os.path.abspath(__file__))
	if not os.path.exists(current_dir + '/database/user_database'):
		return

	with open(current_dir + '/database/user_database', 'rb') as f:
		database = pickle.load(f)
		database.logged_in[name] = status
		print(f"User {name} is logged in: {status}", file=sys.stderr)
		with open(current_dir + '/database/user_database', 'wb') as f:
			pickle.dump(database, f)

def handleLogin():
	print(f"handleLogin", file=sys.stderr)
	username = form.getvalue('username')
	password = form.getvalue('password')
	firstname = form.getvalue('firstname')

	if username == None:
		print(f"Login page requested", file=sys.stderr)
		sess = check_session()
		if sess:
			print(f"Session found: {sess.getSid()}, {sess.name}", file=sys.stderr)
			username = sess.name
			if isLoggedIN(username, sess.getSid()):
				printHomePage()
		printLogin()
	elif firstname == None:
		print(f"Login requested", file=sys.stderr)
		session = authUser(username, password)
		if(session == None):
			printUserMsg("Failed To Login, Username or Passowrd is wrong!")
			return
		print(f"user: {username}, pass: {password}, session: {session.getSid()}", file=sys.stderr)
		cook.clear()
		cook["SID"] = session.getSid()
		cook["SID"]["expires"] = 120
		print("Set-cookie: SID=" + session.getSid())
		print(f"Cookies: {cook}", file=sys.stderr)
		setLoggedIN(username, True)
		printHomePage()
	else :
		print(f"Register requested", file=sys.stderr)
		current_dir = os.path.dirname(os.path.abspath(__file__))
		if os.path.exists(current_dir + '/database/user_database'):
			with open(current_dir + '/database/user_database', 'rb') as f:
				database = pickle.load(f)
				if username in database.user_pass:
					printUserMsg("Username is already Registerd !")
				else:
					database.addUser(username, password, firstname)
					printUserMsg("Account Registerd Successfully!")
		else:
			database = UserDataBase()
			if username in database.user_pass:
				printUserMsg("Username is already Registerd !")
			else:
				database.addUser(username, password, firstname)
				printUserMsg("Account Registerd Successfully!")

def printLogin():
	print(f"printLogin", file=sys.stderr)
	print("Content-Type: text/html\r\n")
	html_content = read_html_file('login.html')
	print(html_content)

def read_html_file(file_name):
	current_dir = os.path.dirname(os.path.abspath(__file__))
	file_path = os.path.join(current_dir, file_name)
	with open(file_path, 'r') as file:
		return file.read()

def check_auth():
	if 'HTTP_COOKIE' in os.environ:
		if "SID" in cook:
			current_dir = os.path.dirname(os.path.abspath(__file__))
			if not os.path.exists(current_dir + '/sessions'):
				os.makedirs(current_dir + '/sessions')
				print(f"Session directory not found", file=sys.stderr)
				return None
			if os.path.exists(current_dir + '/sessions/session_' + cook["SID"].value):
				with open(current_dir + '/sessions/session_' + cook["SID"].value, 'rb') as f:
					sess = pickle.load(f)
					if sess != None:
						return sess.name
			else:
				print(f"Session file not found session_{cook['SID'].value}", file=sys.stderr)
		else:
			print(f"SID not found in cookie", file=sys.stderr)
	else:
		print(f"Cookie not found", file=sys.stderr)
	return None

def check_session():
	if 'HTTP_COOKIE' in os.environ:
		if "SID" in cook:
			current_dir = os.path.dirname(os.path.abspath(__file__))
			if os.path.exists(current_dir + '/sessions/session_' + cook["SID"].value):
				with open(current_dir + '/sessions/session_' + cook["SID"].value, 'rb') as f:
					sess = pickle.load(f)
					if sess != None:
						return sess
	return None

def get_user_database():
	current_dir = os.path.dirname(os.path.abspath(__file__))

	if os.path.exists(current_dir + '/database/user_database'):
		with open(current_dir + '/database/user_database', 'rb') as f:
			return pickle.load(f)
	return UserDataBase()

def get_user_files(username):
	db = get_user_database()
	if not db:
		return
	return db.user_files.get(username, [])

def printHomePage():
	print(f"printHomePage", file=sys.stderr)
	username = check_auth()
	if not username or username == "":
		handleLogin()
		return

	db = get_user_database()
	if db and username in db.user_files:
		if not db.logged_in[username]:
			handleLogin()
			return

	print("Content-type: text/html\r\n")
	html_content = read_html_file('home.html')

	print(f"Username: {username}", file=sys.stderr)
	html_content = html_content.replace('<span id="username"></span>', username)

	user_files = get_user_files(username)
	file_list_html = ""
	for file in user_files:
		file_list_html += f"<li>{file}</li>"

	html_content = html_content.replace('<!-- File list will be populated here -->', file_list_html)

	print(html_content)

def handle_upload():
	sess = check_session()
	if not sess:
		print(f"Post: no session found", file=sys.stderr)
		handleLogin()
		return
	username = check_auth()
	if not username or username == "":
		print(f"Post: no username found", file=sys.stderr)
		handleLogin()
		return
	
	print(f"Content-length: {os.environ.get('CONTENT_LENGTH', 0)}", file=sys.stderr)

	# form = cgi.FieldStorage()
	if "file" not in form:
		return "No file was uploaded"
	
	fileitem = form["file"]
	if not fileitem.file:
		print(f"No file was uploaded", file=sys.stderr)
		return "No file was uploaded"

	filename = os.path.basename(fileitem.filename)
	current_dir = os.path.dirname(os.path.abspath(__file__))
	upload_dir = os.path.join(current_dir, 'uploads', username)
	os.makedirs(upload_dir, exist_ok=True)
	filepath = os.path.join(upload_dir, filename)

	print(f"Uploading file to {filepath}", file=sys.stderr)
	try:
		with open(filepath, 'wb') as f:
			print(f"Writing file to {filepath}", file=sys.stderr)
			if fileitem.file:
				shutil.copyfileobj(fileitem.file, f)
		# Add file to user's file list
		db = get_user_database()
		if username in db.user_files:
			if filename not in db.user_files[username]:
				db.user_files[username].append(filename)
		with open(current_dir + '/database/user_database', 'wb') as f:
			pickle.dump(db, f)
		return f"File '{filename}' uploaded successfully to {upload_dir}"
	except IOError:
		print(f"Error occurred while writing the file", file=sys.stderr)
		return "Error occurred while writing the file"

def handle_delete():
	sess = check_session()
	if not sess:
		print(f"Delete: no session found", file=sys.stderr)
		handleLogin()
		return
	username = check_auth()
	if not username or username == "":
		print(f"Delete: no username found", file=sys.stderr)
		handleLogin()
		return

	filename = form.getvalue("filename")
	if not filename:
		return "No filename provided"
	
	current_dir = os.path.dirname(os.path.abspath(__file__))
	upload_dir = os.path.join(current_dir, 'uploads', username)
	filepath = os.path.join(upload_dir, filename)
	if not os.path.exists(filepath):
		return f"File {filename} does not exist"
	os.remove(filepath)
	if os.path.exists(filepath):
		return f"Failed to delete file {filename}"
	# Remove file from user's file list
	db = get_user_database()
	if not db:
		return "Failed to delete file"
	if username in db.user_files:
		if filename in db.user_files[username]:
			db.user_files[username].remove(filename)
	with open(current_dir + '/database/user_database', 'wb') as f:
		pickle.dump(db, f)
	return f"File {filename} deleted successfully"

def handleLogout():
	username = check_auth()
	if not username or username == "":
		handleLogin()
		return
	setLoggedIN(username, False)
	printHomePage()

def printUserMsg(msg):
	if not msg:
		return
	print("Content-type: text/html\r\n")
	html_content = read_html_file('user_message.html')
	html_content = html_content.replace('<h1 id="message"></h1>', f'<h1 id="message">{msg}</h1>')
	print(html_content)

def main():
	if 'HTTP_COOKIE' in os.environ:
		if "SID" in cook:
			print(f"MAIN {cook['SID'].value}", file=sys.stderr)
		else :
			print(f"MAIN no SID", file=sys.stderr)
	else:
		print(f"MAIN no cookie", file=sys.stderr)
	if os.environ['REQUEST_METHOD'] == 'GET':
		print(f"GET request", file=sys.stderr)
		printHomePage()
	elif os.environ['REQUEST_METHOD'] == 'POST':
		print(f"POST request", file=sys.stderr)
		if "file" in form:
			result = handle_upload()
			printUserMsg(result)
		elif "filename" in form:
			result = handle_delete()
			printUserMsg(result)
		elif "username" in form:
			handleLogin()
		elif "logout" in form:
			handleLogout()
		else:
			result = "Invalid form data"
			printUserMsg(result)
	else:
		print(f"Invalid method", file=sys.stderr)
		result = "Invalid method"
		printUserMsg(result)

form = cgi.FieldStorage()

if 'HTTP_COOKIE' in os.environ:
	cook = Cookies.SimpleCookie()
	cook.load(os.environ.get('HTTP_COOKIE', ''))
else:
	print(f"Cookie not found", file=sys.stderr)
	handleLogin()
main()