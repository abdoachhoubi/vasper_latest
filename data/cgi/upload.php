#!/usr/bin/php-cgi
<?php

// Check if the form has been submitted
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Check if a file was uploaded successfully
	// check all the items in the associative array $_FILES
    if (isset($_FILES['uploaded_file']) && $_FILES['uploaded_file']['error'] === UPLOAD_ERR_OK) {
        $uploadDir = './uploads/'; // Set the directory where you want to store uploaded files

        // Get the uploaded file's information
        $fileName = $_FILES['uploaded_file']['name'];
        $fileSize = $_FILES['uploaded_file']['size'];
        $fileTmpName = $_FILES['uploaded_file']['tmp_name'];

        // Move the uploaded file to the desired directory
        $targetPath = $uploadDir . $fileName;
        if (move_uploaded_file($fileTmpName, $targetPath)) {
            echo "File uploaded successfully!<br>";
            echo "File Name: " . $fileName . "<br>";
            echo "File Size: " . $fileSize . " bytes<br>";
            echo "Temporary Location: " . $fileTmpName . "<br>";
        } else {
            echo "Error uploading the file.<br>";
        }
    } else {
        echo "File upload error.<br>";
    }
}

// Display the HTML form for file uploading
?>

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