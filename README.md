clm
===

php配置加载管理扩展

做三件事
一. 提供进程本地内存的get/set接口(不支持对象和资源)
    bool clm_set(string $key, mixed $value);
    mixed clm_get(string $key);
二. 提供配置文件配置及函数方式的注册接口
    配置方式:
        clm.cfgfiles="<key1>,<filename1>,<handler1>;<key2>,<filename2><handler2>;..."
        该配置项的changeable为PHP_INI_ALL
    接口方式:
        bool clm_cfg_file_register(string $key, string $filename, $handler);
        bool clm_cfg_string_register(string $key, string $cfg_string, $handler);
    配置处理器接口规范:
        array = cfg_handler(string $cfg_string);
三. 提供常用配置文件的解析器: ini, json. 由于初期不支持对象存储, 所以暂不考虑xml


未来:
    1. 考虑对对象的存储支持, 以方便xml支持
    2. 考虑让clm_set/clm_get的key支持保留字符作为分隔符, 可以细粒度操作其存储的数据, 比如:
        clm_set('a.b.c', array('d.e.f' => 8));
        echo clm_get('a.b.c.d.e.f');  #得到结果为int(8)
        clm_set('a.b.c.e.f' => 9);
        print_r(clm_get('a.b.c.d.e')); #得到结果为array('f' => 9)
