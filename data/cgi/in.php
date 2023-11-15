<?php
// Check if the form is submitted
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Set a cookie with the value from the form
    $cookie_name = "test_cookie";
    $cookie_value = $_POST["cookie_value"];
    setcookie($cookie_name, $cookie_value, time() + 3600, "/"); // Cookie expires in 1 hour
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cookie Test</title>
</head>
<body>

<?php
// Retrieve and display the cookie value
if(isset($_COOKIE[$cookie_name])) {
    echo "Cookie '" . $cookie_name . "' is set!<br>";
    echo "Value: " . $_COOKIE[$cookie_name];
} else {
    echo "Cookie '" . $cookie_name . "' is not set!";
}
?>

<!-- HTML form to set a custom value for the cookie -->
<form method="post" action="<?php echo $_SERVER['PHP_SELF']; ?>">
    <label for="cookie_value">Enter Cookie Value:</label>
    <input type="text" id="cookie_value" name="cookie_value">
    <input type="submit" value="Set Cookie">
</form>

</body>
</html>
