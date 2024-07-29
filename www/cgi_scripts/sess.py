#!/usr/bin/env python

import sha, time, Cookie, os

cookie = Cookie.SimpleCookie()
string_cookie = os.environ.get('HTTP_COOKIE')

# If new session
if not string_cookie:
    # The sid will be a hash of the server time
    sid = sha.new(repr(time.time())).hexdigest()
    # Set the sid in the cookie
    cookie['sid'] = sid
    # Will expire in a year
    cookie['sid']['expires'] = 12 * 30 * 24 * 60 * 60
    print 'set-cookie: sid='+sid
# If already existent session
else:
    cookie.load(string_cookie)
    sid = cookie['sid'].value

print 'Content-Type: text/html\n'
print '<html><head><style>'
print '* {margin:0px;padding:0px;box-sizing:border-box;}html{background-color:#fff;}\
			body {height:100vh;width:100%;display:flex;align-items:center;row-gap:1em;\
				justify-content:center;flex-direction:column;font-family:monospace;}\
			h1 {color:#000;font-size:10em;font-style:italic;text-shadow:5px 5px #7a7a7a;}\
			h2 {color:#7a7a7a;text-transform:uppercase;font-size:3em;}a{font-size:20px;}'
print '</style></head><body>'

if string_cookie:
    print '<h1>Already existent session</h1>'
else:
    print '<h1>New session</h1>'

print '<h2>SID =', sid, '</h2>'
print '</body></html>'

