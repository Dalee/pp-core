<?php

/**
 *  XHTML!
 *
 */

class PXHTML {
    public static function tag($name, $params = null, $contents = null) {
        $tag = "<{$name}";

        if($params)
            $tag .= self::build_params_string($params);

        if ($contents) {
            $tag .= '>' . $contents . "</{$name}>";
        } else {
            $tag .= ' />';
        }

        return $tag . "\n"; //return readable xhtml
    }

    /* private */

    /**
     *  array('type'=>'checkbox', 'id'=>12345, 'readonly'=>false, 'disabled'=>true)
     *
     *  will return
     *
     *  " type='checkbox' id='12345' disabled='disabled'" string
     *
     */
    private static function build_params_string($params) {

        $params_string = '';

        foreach($params as $name => $value) {
            if ($value === false)
                continue;

            if ($value === true) {
                $params_string .= " {$name}='{$name}'";
            } else {
                $value = strtr($value, "'", "&apos;");
                $params_string .= " {$name}='{$value}'";
            }
        }

        return $params_string;
    }//~
}//class


/**
 *
 *  PXFormFor 
 *
 *  * usage example *
 *  
 *  $object = array('login'=>'insa', 'password' => '12345');
 *
 *  $form = new PXFormFor('/action.phtml', $object);
 *  
 *  $html = $form->construct('<p><b>username: </b>' . $form->text_field('login') . '</p>' .
 *                           '<p><b>password: </b>' . $form->password_field('password') . '</p>');
 *
 *  * should return *
 *
 *  <form action='/action.phtml' method='POST' 'enctype' => 'multipart/form-data'>
 *  <p><b>username: </b><input type='text' name='login' value='insa' /></p>
 *  <p><b>password: </b><input type='password' name='password' value='12345' /></p>
 *  </form>
 *
 */

class PXFormFor {
    
    private $def_form_params = array('method' => 'POST', 'enctype' => 'multipart/form-data');

    public function __construct($action, $object, $params = array()) {
        $this->action = $action;
        $this->object = $object;
        $this->params = $this->def_form_params + $params;
    }
    
    public function __call($name, $argv) {
        switch (count($argv)) {
            case 1:
                return PXForm::$name($argv[0], $this->object[$argv[0]]);
            case 2:
                return PXForm::$name($argv[0], $this->object[$argv[0]], $argv[1]);
            default:
                throw new Exception("Can't call function {$name} in " . __FILE__ . " at " . __LINE__);
        }
    }

    public function construct($content) {
        PXForm::form($this->action, $content, $params['method'], $params);
    }
    
    public function select($name, $options, $params = array()) {
        return PXForm::select($name, $this->object[$name], $options, $params);   
    }

/* {{{ replaced by __call(...) method

    public function text_field($name, $params = array()) {
        return PXForm::text_field($name, $this->object[$name], $params);
    }

    public function password_field($name, $params = array()) {
        return PXForm::password_field($name, $this->object[$name], $params);
    }

    public function check_box($name, $params = array() {
        return PXForm::check_box($name, $this->object[$name], $params);
    }

    public function text_area($name, $params = array() {
        return PXForm::text_area($name, $this->object[$name], $params);
    }
    
    public function submit($name, $params = array() {
        return PXForm::submit($name, $params);
    }
}}} */


}

/**
 *  Example
 *
 *  $form = PXForm::form('add',
 *                       PXForm::text_field('login', $entered_login) .
 *                       PXForm::password('password', $entered_password),
 *                       'POST');
 *
 *  will return
 *
 *  <form name="add" method="POST'>
 *  <input type='text' name='login' value='...' />
 *  <input type='password' name='password' value='...' />
 *  </form>
 */

class PXForm {

    public static function form($action, $content, $method = 'GET', $params = array()) {
        $params = array('action'=>$action, 'method' => $method) + $params;

        return PXHTML::tag('form', $params, $content);
    }

    public static function text_field($name, $value, $params = array()) {
        return self::input('text', $name, $value, $params);
    }

    public static function check_box($name, $value, $params = array()) {
        return self::input('checkbox', $name, $value, $params);
    }

    public static function password_field($name, $value, $params = array()) {
        return self::input('password', $name, $value, $params);
    }

    public static function hidden_field($name, $value, $params = array()) {
        return self::input('hidden', $name, $value, $params);
    }
    
    public static function text_area($name, $contents, $params = array()) {
        $params = array('name' => $name) + $params;
        return PXHTML::tag('textarea', $params, $contents);
    }
    
    public function select($name, $value, $options, $params = array()) {
        $params = array('name' => $name) + $params;
        return PXHTML::tag('select', $params, self::options_for($value, $options));
    }

    public function options_for($value, $options) {
        $opts = '';
        foreach($options as $val => $text) {
            $params = ($val == $value) ? array('selected' => true) : null;
            $opts .= PXHTML::tag('select', $params, $text);
        }
        return $opts;
    }

    public static function submit($name, $params = array()) {
        return self::input('submit', $name, $name, $params);
    }

    public static function file($name, $value, $params = array()) {
        return self::input('file', $name, $value, $params);
    } 

    /* private */

    private static function input($type, $name, $value, $params = array()) {
        $params = array('type' => $type, 'name' => $name, 'value' => $value) + $params;
        return PXHTML::tag('input', $params);
    }

}

?>
