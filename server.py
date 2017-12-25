if __name__ == '__main__':
    import http.server
    addr = ('', 8000)
    httpd = http.server.HTTPServer(addr, http.server.CGIHTTPRequestHandler)
    httpd.serve_forever()
