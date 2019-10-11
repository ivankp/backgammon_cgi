<table class="games">
<?php
  $games = sqlite("
    SELECT g.id, p1.username, p2.username, t.short
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
