#!/usr/bin/env python3

import os
import sys
import urllib.parse

print("Content-Type: text/html")    # HTML is following
print()                             # Blank line, end of headers

print("<html><body>")
print("<h1>Hello, World!</h1>")

# Traiter les données de la requête GET
if os.environ['REQUEST_METHOD'] == 'GET':
    query_string = os.environ['QUERY_STRING']
    parameters = urllib.parse.parse_qs(query_string)
    print("<p>Paramètres GET:</p>")
    for param, values in parameters.items():
        for value in values:
            print(f"<p>{param}: {value}</p>")

# Traiter les données de la requête POST
elif os.environ['REQUEST_METHOD'] == 'POST':
    content_length = int(os.environ['CONTENT_LENGTH'])
    post_data = sys.stdin.read(content_length)
    parameters = urllib.parse.parse_qs(post_data)
    print("<p>Paramètres POST:</p>")
    for param, values in parameters.items():
        for value in values:
            print(f"<p>{param}: {value}</p>")

print("</body></html>")
