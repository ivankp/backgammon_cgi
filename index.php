<?php
function get($var, $default=null) { return isset($var) ? $var : $default; }
$db = 'sqlite3 db/database.db ';
$cookie_name = 'login';

if (isset($_POST['logout'])) { // logout
  setcookie($cookie_name, '', time()-3600);
  header('Location: .');
  exit;
}

$login_success = false;
$cookie = get($_COOKIE[$cookie_name]);
$username = get($_POST['username'],$cookie);
if ($username) { // attempt login
  $uid = exec($db."'SELECT id FROM users WHERE username=\"$username\"'");
  if (!empty($uid)) {
    $login_success = true;
    if ($cookie != $username) // set login cookie
      setcookie($cookie_name,$username);
  }
}

if ($login_success) {
  if (isset($_GET['g'])) { /* Load next game */
    $g = $_GET['g'];
    if (!is_numeric($g)) {
      $g = exec($db.
        "'SELECT id FROM games WHERE finished!=1 AND".
        "(player1=$uid and turn=1) OR (player2=$uid and turn=2) LIMIT 1'");
      header('Location: '.(is_numeric($g) ? "?g=$g" : '.'));
      exit;
    }
  }
}
?>
<!DOCTYPE HTML>
<html lang="en-US">
<head>
<title>Backgammon</title>
<link rel="stylesheet" href="styles.css" type="text/css">
<link rel="stylesheet" href="styles/default.css" type="text/css">
<script src="jquery-3.3.1.min.js"></script>
</head>
<body>

<?php if (!$login_success) { ?>

<p>Need to log in</p>
<form method="post">
  <label>username: <input type="text" name="username"></label>
  <input type="submit" value="Login">
</form>

<?php
  if ($username) echo "<p>$username is not a valid username.</p>";

} else { /* Already logged in */
  include 'nav.php';

  if (isset($_GET['g'])) { /* Load a game */
?>

<script src="backgammon.js" type="text/javascript"></script>
<script>
$(() => {
  load_game(<?php echo $g; ?>)
  .done(game => { board_setup(game); });
});
</script>

<div id="container">
<div id="left">

<div id="info">
<p>Game: <?php echo $g; ?></p>
<p>&#9675; <span id="white_player"></span></p>
<p>&#9679; <span id="black_player"></span></p>
</div>

</div>
<div id="board"></div>
</div>
<div id="under"></div>

<?php } else { /* List of games */
  include 'games.php';
} } ?>

</body>
</html>
