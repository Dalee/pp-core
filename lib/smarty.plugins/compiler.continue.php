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
function smarty_compiler_continue($level, &$smarty)
{
    !ctype_digit((string) $level) && $level = '';
    return "\ncontinue $level;";
}
