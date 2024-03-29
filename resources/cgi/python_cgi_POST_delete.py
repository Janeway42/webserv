#!/usr/bin/python

import time
import cgitb
import cgi
import os
import sys # to read from std input
cgitb.enable()	# detailed errors msgs


# GET, POST, PUT and DELETE (there are others) are a part of the HTTP standard.
# HTML only defines the use of POST and GET for forms.

for param in os.environ.keys():
    if param == 'UPLOAD_DIR':
        uploadDir_AbsPath = os.environ[param]
    if param == 'PATH_INFO':
        uploadDirName = os.environ[param]


form = cgi.FieldStorage()

# Define the path to the folder you want to list
rootFolder = os.getcwd()

for param in os.environ.keys():
    # print("<b>%30s</b>: %s</br>") % (param, os.environ[param])
    # sys.stderr.write(param + ':  ')
    # sys.stderr.write(os.environ[param] + '\n')
	if param == 'REQUEST_METHOD':
		URL = os.environ[param]
        # If it is a POST request, we can get the form from the url
		if URL == "POST":
            # print("<p>FOUND METHOD: " + URL + "<br>")
            # If the form has a delete key on it, we can get the string after this key, which is the file to be deleted
			if form["delete"] is not None:
				formData = form["delete"].value
				try:
					os.remove(os.path.join(uploadDir_AbsPath, form["delete"].value))
				except FileNotFoundError:
					message = 'Error: file not found.'

# Define the HTML template
html_template = """
<!DOCTYPE html>
<html>
<head>
	<title>Uploaded Files</title>
	<style>
		.button-container {{
			display: flex;
		}}
  		.button-container a {{
			color: black;
			text-decoration: none;
		text
		}}
	</style>
</head>
<body>
	<h4><a href='../cgi_index.html'> Main Cgi Page </a></h4>
	<h3>This is the file python_cgi_POST_upload.py</h3>
	<hr>
	<h2>Uploaded Files:</h2>
	<br>
	<fieldset style="background: #fff3f3; border: 2px solid #fbd58f;">
		<legend> DELETE FILE/IMAGE (method POST) </legend>
		<ul>
			{}
		</ul>
	</fieldset>
</body>
</html>
"""

# <input type="hidden" name="delete" value="{}">
# Define the item HTML template

item_template = """
<form action="python_cgi_POST_delete.py" method="post">
	<mark><b>{}</b></mark>
	<div class="button-container">
		<button> <a href="../{}/{}">  View </a> </button> 
		<button> <a href="../{}/{}" download> Download </a> </button>
		<input type="hidden" name="delete" value="{}">
		<input type="submit" value="Delete">
	</div>
</form>
<br>
"""

# Generate the item HTML
items_html = ""
for item in os.listdir(uploadDir_AbsPath):
	item_path = os.path.join(uploadDir_AbsPath, item)
	item_html = item_template.format(item, uploadDirName, item, uploadDirName, item, item_path)
	# item_html = item_template.format(item, item, item, item_path)
	items_html += item_html

# Generate the full HTML page
html = html_template.format(items_html)

# Set the content type to HTML
# print("Content-type:text/html\r\n\r\n")

# Output the HTML page
print(html)
