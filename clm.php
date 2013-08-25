<?php
class A {
    public static function f1() {
        echo 'A::f1()' . chr(10);
    }
    public function f2() {
        echo 'A::f2()' . chr(10);
    }
}
$a = new A();
clm_cfg_register('a', 'b', 'debug_print_backtrace', 'debug_print_backtrace');
clm_cfg_register('a', 'b', array('A', 'f1'), 'debug_print_backtrace');
clm_cfg_register('a', 'b', array($a, 'f2'), 'debug_print_backtrace');
/*
$a = array(1, 2, 3);
$b = clm_set('hello', $a);
var_dump($b);
*/
