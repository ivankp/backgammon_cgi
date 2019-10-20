<table class="games">
<?php
  exec($db."'
    SELECT g.id, p1.username, p2.username, t.short, g.turn
    FROM games AS g
    JOIN users AS p1 ON g.player1 = p1.id
    JOIN users AS p2 ON g.player2 = p2.id
    JOIN game_types AS t ON g.game_type = t.id
    WHERE p1.username = \"$username\" OR p2.username = \"$username\"
  '",$games);
  echo "<tr><td>Turn</td><td>Opponent</td><td>Type</td></tr>\n";
  foreach ($games as $game) {
    $game = explode('|',$game);
    $opp = $game[1]!=$username ? 1 : 2;
    $myturn = $game[4]!=$opp;
    echo "<tr>";
    echo "<td><a href=\"?g=$game[0]\" class=\"".
      ($myturn ? 'myturn' : 'oppturn')
      ."\">".
      ($myturn ? 'Yes' : 'Opp')
      ."</a></td>";
    echo "<td>".$game[$opp]."</td>";
    echo "<td>$game[3]</td>";
    echo "</tr>\n";
  }
?></table>
