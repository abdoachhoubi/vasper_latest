#!/usr/bin/env python
import cgi
import os

def save_uploaded_file(file_item):
    upload_dir = '/nfs/homes/ibouchaf/Desktop/webserv/data/cgi/uploads/'  # Set the directory where you want to store uploaded files

    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)

    file_name = os.path.basename(file_item.filename)
    target_path = os.path.join(upload_dir, file_name)

    with open(target_path, 'wb') as new_file:
        while True:
            chunk = file_item.file.read(8192)
            if not chunk:
                break
            new_file.write(chunk)

    return file_name

def main():
    form = cgi.FieldStorage()

    if "uploaded_file" in form:
        file_item = form["uploaded_file"]
        
        if file_item.filename:
            try:
                file_name = save_uploaded_file(file_item)
                print("Content-Type: text/html\r\n\r\n")
                print("File uploaded successfully!<br>")
                print("File Name: " + file_name + "<br>")
                print("File Size: " + str(os.path.getsize('/nfs/homes/ibouchaf/Desktop/webserv/data/cgi/uploads/' + file_name)) + " bytes<br>")
            except Exception as e:
                print("Content-Type: text/html\r\n\r\n")
                print("Error uploading the file.<br>")
        else:
            print("Content-Type: text/html\r\n\r\n")
            print("File upload error.<br>")
    else:
        print("Content-Type: text/html\r\n\r\n")
        print("""
        <!DOCTYPE html>
        <html>
        <head>
            <title>File Upload Form</title>
        </head>
        <body>
            <h1>Upload a File</h1>
            <form action="" method="post" enctype="multipart/form-data">
                <label for="uploaded_file">Select a file:</label>
                <input type="file" name="uploaded_file" id="uploaded_file">
                <input type="submit" value="Upload">
            </form>
        </body>
        </html>
        """)

if __name__ == "__main__":
    main()
