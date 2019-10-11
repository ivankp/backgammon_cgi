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
$username = get($_POST['username']);
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

<?php if (!isset($_COOKIE['login']) && !$login_success) { ?>

<p>Need to log in</p>
<form method="post">
  <label>username: <input type="text" name="username"></label>
  <input type="submit" value="Login">
</form>

<?php
  if ($username) echo "<p>$username is not a valid username.</p>";

} else { /* Already logged in */
  if (!$username) $username = $_COOKIE['login'];
?>
<form method="post">
  <input type="submit" value="Logout" name='logout'>
</form>

<p><?php echo $username; ?></p>
<p><a href=".">List of games</a></p>

<?php if (!isset($_GET['g'])) { /* List of games */ ?>

<table>
<?php
  $games = sqlite("
    SELECT g.id, p1.username, p2.username, t.name
    FROM games AS g
    JOIN users AS p1 ON g.player1 = p1.id
    JOIN users AS p2 ON g.player2 = p2.id
    JOIN game_types AS t ON g.game_type = t.id
    WHERE p1.username = \"$username\" OR p2.username = \"$username\"
  ");
  echo "<tr><td>Game</td><td>Opponent</td><td>Type</td></tr>\n";
  foreach ($games as $game) {
    $game = explode('|',$game);
    echo "<tr>";
    echo "<td><a href=\"?g=$game[0]\">$game[0]</a></td>";
    if ($game[1]!=$username) echo "<td>$game[1]</td>";
    else echo "<td>$game[2]</td>";
    echo "<td>$game[3]</td>";
    echo "</tr>\n";
  }
?></table>

<?php } else { ?>

<p>Game <?php echo $_GET['g']; ?></p>

<script src="backgammon.js" type="text/javascript"></script>
<script>
$(() => {
  load_game(<?php echo $_GET['g']; ?>)
  .done(game => { board_setup(game); });
});
</script>

<div id="board"></div>

<?php } ?>
<?php } ?>

</body>
</html>
