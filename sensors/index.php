<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<title>Sensors</title>

		

<?php
session_start();

$MINUTEINTERVAL=30;
if(!isset($_GET['days']) or !is_numeric($_GET['days']) or $_GET['days']>365 or $_GET['days']<1) $days=90;
else $days=addslashes((int)($_GET['days']));
if(!isset($_GET['interval']) or !is_numeric($_GET['interval']) or $_GET['interval']>30 or $_GET['interval']<5) {/*$MINUTEINTERVAL=30;*/}
else $MINUTEINTERVAL=addslashes((int)($_GET['interval']));

$passarray=array();
$passarray[]="";
$passarray[]=md5("");
if (!empty($_SERVER['HTTP_CLIENT_IP'])) $ip = $_SERVER['HTTP_CLIENT_IP'];
elseif (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) $ip = $_SERVER['HTTP_X_FORWARDED_FOR'];
else $ip = $_SERVER['REMOTE_ADDR'];
if(isset($_POST['send'])) $_SESSION['password']=md5($_POST['pass']);
if((!isset($_SESSION['password']) or $_SESSION['password']=="" or !in_array($_SESSION['password'],$passarray)) and $ip!="0.0.0.0"/*<===MyIP*/){
	echo '</head><body style="text-align:center">';
	echo '<font color="red"><b>Нет доступа</b></font><br><br>';
	echo '<form action="?" method="POST">Пароль: <br><input type="password" name="pass"><br><br><input type="submit" name="send" value="Войти"></form>';
	
	echo '</body></html>';
	die();
}
include("config.php");
$conn=dbcn();
$query="SELECT * FROM `data` WHERE `date`>DATE_SUB(NOW(), Interval $days day) and (MINUTE(`date`) mod ".$MINUTEINTERVAL."=0 or `date`=(SELECT MAX(`date`) FROM `data`)) ORDER BY `data`.`date` ASC";
$result=mysqli_query($conn,$query);
dbcl($conn);
while($row=mysqli_fetch_array($result)){
	//$dates[]=date("d.m H:i:s",strtotime($row['date']));
	$dates[]=$row['date'];
	//$datatemp[$row['type']][date("d.m H:i:s",strtotime($row['date']))]=$row['value'];
	$datatemp[$row['type']][$row['date']]=$row['value'];

}
$dates=array_unique($dates);
foreach($dates as $key=>$val){
	if(isset($datatemp[1][$val])) $dataval[1][]=$datatemp[1][$val]; else $dataval[1][]=$dataval[1][count($dataval[1])-1];
	if(isset($datatemp[2][$val])) $dataval[2][]=$datatemp[2][$val]; else $dataval[2][]=$dataval[2][count($dataval[2])-1];
	if(isset($datatemp[3][$val])) $dataval[3][]=$datatemp[3][$val]; else $dataval[3][]=$dataval[3][count($dataval[3])-1];
}
unset($datatemp);
//foreach($dates as $key=>$val) $dates[$key]=date("d.m",strtotime($val));
$lastval[1]=end($dataval[1]);
$lastval[2]=end($dataval[2]);
$lastval[3]=end($dataval[3]);
?>
<!DOCTYPE HTML>
	</head>
	<body>
		<div id="chart" style="min-width: 310px; height: 400px; margin: 0 auto"></div>
		<table  style="width:100%;text-align:center;" border="1px"><!--<tr><td colspan="3">Текущие значения--><tr style="font-size:50px"><td style="width:33%;padding: 25px;">Датчик 1<td style="width:33%;padding: 25px;">Датчик 2<td style="width:33%;padding: 25px;">Влажность<tr style="font-size:75px"><td style="width:33%;padding: 50px 0px;color:#008cf0"><b><?=$lastval[1]?> °C</b><td style="width:33%;padding: 50px 0px;color:#ffa500"><b><?=$lastval[2]?> °C</b><td style="width:33%;padding: 50px 0px;color:#3cb371"><b><?=$lastval[3]?> %</b></table>
		
		<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js"></script>
		<script src="/sensors/js/charts.js"></script>
		<script type="text/javascript">
		function paint(){
			$('#chart').highcharts({
                chart: {
					type: 'spline',
					zoomType: 'x'
				},
				title: {
					text: 'Sensors',
					x: -20 //center
				},
				xAxis: {
					categories: ['<?=implode("','",$dates)?>']
				},
				
				yAxis: {
					title: {
						text: 'Значение'
					},
					plotLines: [{
						value: 0,
						width: 1,
						color: '#808080'
					}]
				},
				tooltip: {
					//valueSuffix: '°C',
					shared: true,
					crosshairs: true
				},
				legend: {
					layout: 'vertical',
					align: 'right',
					verticalAlign: 'middle',
					borderWidth: 0
				},
				series: [{
					name: 'Температура датчик 1',
					data: [<?=implode(",",$dataval[1])?>],
					tooltip: {
						valueSuffix: ' °C'
					},
					marker: {
						//enabled: false,
						symbol: 'circle'
					},
					color: '#008cf0'
				}, {
					name: 'Температура датчик 2',
					data: [<?=implode(",",$dataval[2])?>],
					tooltip: {
					valueSuffix: ' °C'
					},
					marker: {
						//enabled: false,
						symbol: 'circle'
					},
					color: '#ffa500'
				}, {
					name: 'Влажность',
					data: [<?=implode(",",$dataval[3])?>],
					tooltip: {
						valueSuffix: ' %'
					},
					marker: {
						//enabled: false,
						symbol: 'circle'
					},
					color: '#3cb371'
				}]
			});
		};

		$(function () {
			paint();
		});
	</script>
	</body> 
</html>
