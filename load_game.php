<?php
echo exec('python load_game.py db/database.db '.$_GET['id'].' 2>&1');
?>
