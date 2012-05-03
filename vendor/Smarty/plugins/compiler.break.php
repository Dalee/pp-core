<?php
/*
 * Smarty plugin
 * -------------------------------------------------------------
 * File:     compiler.break.php
 * Type:     compiler
 * Name:     break
 * Purpose:  Breaking current context
 * -------------------------------------------------------------
 */
function smarty_compiler_break($level, &$smarty)
{
    !ctype_digit($level) && $level = '';
    return "\nbreak $level;";
}
