<?php


class CUSTOMSQLConverter extends Converter
{

    public $name = "CUSTOMSQLConverter";
    public $package = 'default';
	public $mysqlTables = array('class','function','parameter','property',
                   'source','constant','tag');

	static $mysqlConfig = array('host', 'user','pwd', 'database');

    /**
     * 
     * 
     */
    function CUSTOMSQLConverter(&$allp, &$packp, &$classes, &$procpages, 
                                 $po, $pp, $qm, $targetDir, $templateDir, $title) {
		// cleanUpTables
        foreach($this->mysqlTables as $name) {
           $this->query('DELETE FROM doc_'.$name);
        }
        $this->package = preg_replace('/[^a-z0-9]/','-',strtolower($targetDir));
        $this->package = trim($this->package,'-');
        $this->parseClasses($classes);
        $unit=array('b','kb','mb','gb','tb','pb');
        $memFootprint = memory_get_usage(true);
        $memFootprint = @round($memFootprint/pow(1024,
                               ($i=floor(log($memFootprint,1024)))),2);
        $memFootprint.=' '.$unit[$i];
        exit('Done buddy... Memory consumption '.$memFootprint);
    }

    function __destruct() {
        $this->connect(false);
    }
    
	/**
	 *
	 *
	 */
	protected function connect($close=false) {
		static $link=null;
        if($link && $close) return mysql_close();
		if(!is_null($link)) return $link;
		if(!defined('CUSTOMSQLCONVERTER_DBCONFIG')) {
			exit('No db Config, you must define CUSTOMSQLCONVERTER_DBCONFIG');
		}
		$dbconf = json_decode(CUSTOMSQLCONVERTER_DBCONFIG, true);
        mysql_connect($dbconf['host'],$dbconf['user'],$dbconf['pwd'])
            or die('no db connection');
        mysql_select_db($dbconf['database'])
            or die('no db found');
        $link=true;
        return true;
		////$link = new mysqli("vm-develdb.eqx", "documentation", "boc4odAnHayd", "documentation_intra");
		//$link->select_db();
	}
	
    /**
     *
     */
    protected function query($sqlQuery, $fetchOne=false) {
        $this->connect();
        $results = array();
        $sql = mysql_query($sqlQuery);
        if(!$sql) throw new Exception('DB error : '.mysql_error().' for query: '.$sqlQuery);
        if(!is_resource($sql)) return $sql;
        while( $res = mysql_fetch_object($sql) ) {
            if($fetchOne) return $res;
            $results[] = $res;
        }
        mysql_free_result($sql);
        if($fetchOne) return null;
        return $results;
    }
    
    /**
     *
     */
    protected function safe($v,$quote=true) {
        $this->connect();
        if($quote) return "'".mysql_real_escape_string($v)."'";
        else return mysql_real_escape_string($v); 
    }
    
    /**
     * 
     * 
     */
    protected function sqlSave($table,$values,$updateTest=array()) {
        $this->connect();
        if(count($updateTest)) {
            $wheresql = array();
            
            foreach($updateTest as $k) {
                $wheresql[] ="`$k`=".$this->safe($values[$k]);
            }
            $sql = 'SELECT COUNT(*) AS counter 
                    FROM `doc_'.$table.'` 
                    WHERE '.join(' AND ',$wheresql);
            $isUpdate = (bool)$this->query($sql,true)->counter;
        } else {
            $isUpdate = false;
        }
        
        $sql = $isUpdate ? 'UPDATE' : 'INSERT INTO';
        $sql.= " `doc_$table` SET ";
        $attrs=array();
        foreach($values as $k => $v) {
            $attrs[] = "`$k`=".$this->safe($v);
        }
        $sql.= join(', ',$attrs);
        if($isUpdate) {
            $attrs=array();
            foreach($updateTest as $k) {
                $attrs[] = "`$k`=".$this->safe($values[$k]);
            }
            $sql .= " WHERE ".join(' AND ',$attrs);
        }
        return $this->query($sql);
    }
    

    /**
     * 
     * 
     */
    function parseClasses($struct) {
        $classes = array();
        foreach($struct->classesbyfile as $path => $definitions) {
            foreach($definitions as $k => &$v) {
                $this->addClass($v);
            }
        }
        foreach($struct->methodsbyfile as $def) {
            foreach($def as $k => &$v ) {
                foreach($v as $m) $this->addClassMethod($m);
            }
        }
        foreach($struct->varsbyfile as $def) {
            foreach($def as $k => &$v ) {
                foreach($v as $p) $this->addClassProperty($p);
            }
        }
        foreach($struct->constsbyfile as $const) {
            foreach($const as $k => &$v ) {
                foreach($v as $c) $this->addClassConstant($c);
            }
        }
        
    }

    /**
     *
     */
    function getPackage($param) {
        return (string)$this->package;
    }
    
    
    
    function parseDocBlock($docblock) {
        $desc = @$docblock->desc;
        $sdesc = @$docblock->sdesc;
        $desc = is_object($desc) ? $desc->getString() : $desc;
        $desc = is_array($desc) ? $desc[0] : $desc;
        $sdesc = is_object($sdesc) ? $sdesc->getString() : $sdesc;
        $sdesc = is_array($sdesc) ? $sdesc[0] : $sdesc;
        $doc = $desc."\n\n".$sdesc;
        return (string)$doc;
    }

    /**
     * 
     * 
     */  
    function addClass($c) {
        print 'add class '. $c->getName() ."\n";
        $attrs = $c->getModifiers() ? $c->getModifiers() : array();
        $doc = $this->parseDocBlock(@$c->docblock);
        $data = array(
            'package'   => $this->getPackage($c),
            "name"      => $c->getName(),
            'extends'   => $c->extends,
            'file'      => $c->sourceLocation,
            'modifiers'     => join(',',$attrs),
            'doc'       => $doc
        );

        $this->sqlSave('class',$data, array('package','name') );
        foreach($c->docblock->tags as $name => $tag) {
            $this->addDocTag('class', $data['name'], null, $tag);            
        }
    }

    /**
     * 
     * 
     */
    function addClassMethod($method) {

        print 'add method '. $method->getClass().'::'.$method->getName() ."\n";
        $data = array();
        $attrs = $method->getModifiers() ? $method->getModifiers() : array();
        $doc = $this->parseDocBlock(@$method->docblock);
        $data['package'] = $this->getPackage($method);
        $data['class'] = $method->getClass();
        $data['name'] = $method->getName();
        $data['modifiers'] = join(' ',$attrs);
        $data['signature'] = $method->getFunctionCall();
        $data['line'] = $method->getLineNumber();
        $data['doc'] = $doc;
        $data['return'] = isset($method->returns) 
                        ? $method->returns->getType() : '?';
        $this->sqlSave('function', $data, 
                        array('name','package','class'));
		/*
			Add Params
		*/
		if(isset($method->params) && is_array($method->params)) {
			$i=0;
	        foreach($method->params as $k => $p) {
	            $docParams = (isset($method->docblock->params) && isset($method->docblock->params[$k]) )
	                       ? $method->docblock->params[$k]
	                       : new stdClass;
	            $this->addClassMethodParameter($method, $k, $i++, $p, $docParams);
	        }			
		}
		/*
			Add docs tags
		*/
        foreach($method->docblock->tags as $name => $tag) {
            $this->addDocTag('function', $data['class'], $data['name'], 
                              $tag);            
        }
		/*
			Finaly add source 
		*/
		print "  - Grab source in ".$method->path."\n";
		$source = array_slice(file($method->path), 
							  $method->linenumber-1,
							  $method->endlinenumber-$method->linenumber);
		$source = join('',$source);	
		$this->sqlSave('source', array(
                            'package' => $this->getPackage($method),
							'class' => $method->getClass(),
							'method' => $method->getName(),
							'source' => $source
                            ),array('package','class','method'));
    }


    /**
     * 
     * 
     */
    function addClassMethodParameter($method, $name, $pos, $param, $paramDoc) {

        print 'add method params '.$name.' for '. $method->getClass().'::'.$method->getName() ."\n";
        $data = array();

        $doc = $this->getDocTagValue($paramDoc->getValue());
        
        $data['package'] = $this->getPackage($method);
        $data['class'] = $method->getClass();
        $data['function'] = $method->getName();
        $data['name'] = $name;
        $data['type'] = @$paramDoc->returnType;
        $data['doc'] = $doc;
        $data['pos'] = $pos;
        $data['default'] = $param[0];
        
        $this->sqlSave('parameter', $data, 
                        array('package','function','name','class'));

    }


    function addClassProperty($p) {
        
        print 'add property '. $p->getClass().'::'.$p->getName() ."\n";
        $data = array();
        $attrs = $p->getModifiers() ? $p->getModifiers() : array();
        $doc = $this->parseDocBlock(@$p->docblock);
        
        $data['package'] = $this->getPackage($p);
        $data['class'] = $p->getClass();
        $data['name'] = $p->getName();
        $data['modifiers'] = join(' ',$attrs);
        $data['line'] = $p->getLineNumber();
        $data['doc'] = $doc;
        $this->sqlSave('property', $data, array('name','package','class'));
        if(isset($p->docblock->tags)) {
            foreach($p->docblock->tags as $tag) {
                $this->addDocTag('property', $data['class'], $data['name'], 
                                  $tag);            
            }
        }
    }
    

    function addClassConstant($c) {
        
        print 'add constant '. $c->getClass().'::'.$c->getName() ."\n";
        $data = array();
        $doc = $this->parseDocBlock(@$c->docblock);
        $data['package'] = $this->getPackage($c);
        $data['class'] = $c->getClass();
        $data['name'] = $c->getName();
        $data['line'] = $c->getLineNumber();
        $data['doc'] = $doc;
        $this->sqlSave('constant', $data, array('name','package','class'));
        if(isset($p->docblock->tags)) {
            foreach($p->docblock->tags as $tag) {
                $this->addDocTag('constant', $data['class'], $data['name'], 
                                  $tag);
            }
        }
    }
    
    function getDocTagValue($tag, $str='') {
        
        if(is_string($tag)) {
            $str .= $tag;
        } else if(is_array($tag)) {
            foreach($tag as $v) {
                $str = $this->getDocTagValue($v,$str);
            }
        } else {
            $str = $this->getDocTagValue($tag->getValue(),$str);
        }
        return $str;
    }


    function addDocTag($table, $class=null, $name=null, $tag) {

        $data = array();
        $data['table'] = $table;
        $data['package'] = $this->getPackage($class);
        $data['class'] = (string)$class;
        $data['name'] = (string)$name;
        
        $data['tag'] = $tag[0]->keyword;
        $data['value'] = $this->getDocTagValue($tag);
        if(!is_string($data['value'])) {
            print_r($data['value'][0]->getValue());
            $data['value'] = $data['value']->getValue();
        }
        print 'add doctag '. $tag[0]->keyword.' for '.$data['table'].' '.$data['class']
                    .'::'.$data['name']."\n";
        $this->sqlSave('tag', $data, 
                        array('name','package','class','table','tag'));
    }




    /**
     * 
     * 
     */
    function getPageName() {
        return 'page';
    }


    function replaceElement() {
        
    }

}
