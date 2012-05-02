<?php
/*
 * Smarty plugin
 * -------------------------------------------------------------
 * File:     compiler.continue.php
 * Type:     compiler
 * Name:     return
 * Purpose:  Continue for cycles context.
 * -------------------------------------------------------------
 */
function smarty_compiler_continue($tag_arg, &$smarty)
{
    return "\ncontinue;";
}
