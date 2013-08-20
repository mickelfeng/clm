clm
===

php配置加载管理扩展
------------------

<b>做三件事</b><br />
一. 提供进程本地内存的get/set接口(不支持对象和资源)<font color="red">(已完成)</font><br />
    <pre><code>
    bool clm_set(string $key, mixed $value);
    mixed clm_get(string $key);
    </code></pre><br />
二. 提供配置文件配置及函数方式的注册接口<br />
    配置方式:<br />
    <pre><code>
        clm.cfgfiles="&lt;key1&gt;,&lt;filename1&gt;,&lt;handler1&gt;;&lt;key2&gt;,&lt;filename2&gt;&lt;handler2&gt;;..."
        #该配置项的changeable为PHP_INI_ALL<br />
    </code></pre><br />
    接口方式:<br />
    <pre><code>
        bool clm_cfg_file_register(string $key, string $filename, $handler);
        bool clm_cfg_string_register(string $key, string $cfg_string, $handler);
    </code></pre><br />
    配置处理器接口规范:<br />
    <pre><code>
        array = cfg_handler(string $cfg_string);
    </code></pre><br />
三. 提供常用配置文件的解析器: ini, json. 由于初期不支持对象存储, 所以暂不考虑xml<br />
<br />
<br />
<b>未来:</b><br />
    1. 考虑对对象的存储支持, 以方便xml支持<br />
    2. 考虑让clm_set/clm_get的key支持保留字符作为分隔符, 可以细粒度操作其存储的数据, 比如:<br />
    <pre><code>
        clm_set('a.b.c', array('d.e.f' => 8));
        echo clm_get('a.b.c.d.e.f');  #得到结果为int(8)
        clm_set('a.b.c.e.f' => 9);
        print_r(clm_get('a.b.c.d.e')); #得到结果为array('f' => 9)
    </code></pre>
