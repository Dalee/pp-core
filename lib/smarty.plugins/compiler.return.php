<?php
/*
 * Smarty plugin
 * -------------------------------------------------------------
 * File:     compiler.return.php
 * Type:     compiler
 * Name:     return
 * Purpose:  Return from current template.
 * -------------------------------------------------------------
 */
function smarty_compiler_return($tag_arg, &$smarty)
{
    return "\nreturn;";
}
