
<?php 
//# !/usr/bin/php

echo "Start of PhP File\n";

// if ($_SERVER["REQUEST_METHOD"] == "_POST") {
  //     echo "yes\n";
  //     $name = test_input($_POST["surname"]);  # test input value for malicuous
  // }
  
  
 $surname = "Smith\n";
 echo "Surname:  ";
 echo $surname;
 $name = test_input($_POST["surname"]);  # test input value for malicuous code
 


echo "PWD:  ["; 
echo $_ENV["PWD"];
echo "]\n";

echo "HOME:  [";
echo $_ENV["HOME"];
echo "]\n";

// Coming from **array from the 3rd argument
echo "CITY: ["; 
echo $_ENV["CITY"];
echo "]\n";

echo "STREET: [";
echo $_ENV["STREET"];
echo "]\n";

echo "SERVER, HOME: [";
echo $_SERVER["HOME"];
echo "]\n";

echo "SERVER, CITY: [";
echo $_SERVER["CITY"];
echo "]\n";




// Or simply use a Superglobal ($_SERVER or $_ENV)
// $ip2 = $_SERVER['REMOTE_ADDR'];
// $ip2 = $_SERVER['_POST'];
// $ip2 = $_ENV["HOME"];
// echo "&_ENV['HOME']:  [";  echo $ip2;  echo "]\n";
// $ip2 = $_ENV["CITY"];
// echo "&_ENV['CITY']:  [";  echo $ip2;  echo "]\n";


function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}

?>
