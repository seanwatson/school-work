<?php if (!defined('TMPL_DIR')) return; ?>

<form id="inputArea" action="index.php?action=login" method="post">

        <label for="email">Email</label>
        <input type="text" name="email" id="email" value="<?php echo $HTML['email'];?>" />

        <label for="password">Password</label>
        <input type="password" name="password" id="password" value="<?php echo $HTML['password'];?>" />

        <span><?php echo $HTML['login_error'];?></span>

	<input class="submit" type="submit" value="Login" />
	<div class="user">
		New User?  <a href="index.php?action=signup" target="_self">Sign up here</a>
	</div>
		
	<input type="hidden" name="submitted" value="yes" />
</form>
