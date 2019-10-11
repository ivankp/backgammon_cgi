<?php
function get($var, $default=null) { return isset($var) ? $var : $default; }
function sqlite($query) {
  exec('sqlite3 db/database.db \''.$query.'\'',$out);
  return $out;
}

if (isset($_POST['logout'])) { // logout
  setcookie('login', '', time()-3600);
  header('Location: .');
  exit;
}

$login_success = false;
$username = get($_POST['username'],get($_COOKIE['login']));
if ($username) { // attempt login
  $select_user = sqlite(
    "SELECT EXISTS(SELECT 1 FROM users WHERE username=\"$username\")");
  $login_success = '1' == $select_user[0];
  if ($login_success) { // set login cookie
    setcookie('login',$username);
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

  if (!isset($_GET['g'])) { /* List of games */
    include 'games.php';
  } else { /* Load a game */ ?>

<script src="backgammon.js" type="text/javascript"></script>
<script>
$(() => {
  load_game(<?php echo $_GET['g']; ?>)
  .done(game => { board_setup(game); });
});
</script>

<div id="container">
<div id="left">

<div id="info">
<p>Game: <?php echo $_GET['g']; ?></p>
<p>&#9675; <span id="white_player"></span></p>
<p>&#9679; <span id="black_player"></span></p>
</div>

</div>
<div id="board"></div>
</div>
<div id="under"></div>

<?php } } ?>

</body>
</html>
