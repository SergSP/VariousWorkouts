<?php
$config['user']="";
$config['password']="";
$config['db']="";

function dbcn(){
	global $config;
	$db=mysqli_connect('localhost',$config['user'],$config['password'],$config['db']);
	mysqli_set_charset($db,"utf8");
	return $db;
}
function dbcl($db){
	mysqli_close($db);
}
?>