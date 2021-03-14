#!/bin/bash
#脚本实现的功能：
#1）产生一个链接文件/usr/local/AGREE_ABD，指向cd /usr/local/AGREE_ABD_0.1.1-el6目录，
#2）安装做私有yum仓库所需的createrepo工具，建立yum仓库，
#3）安装httpd并把目录/var/www/html/指向安装包的rpm目录，
#4）添加yum源配置文件，
#5）安装ifconfig、expect、ntp、unzip、ansible等工具
#       ntp要配置cmserver为时钟服务器
#6）设置ansible的hosts配置的主机ip，
#7）设置主机hosts的ip、域名列表,
#8）分发主机ssh的key到各个节点、实现ssh免密登陆，
#9）安装cm所依赖的rpm包。
#   准备csd、parcel包
#10) 部署jdk1.8和设置环境变量
#11) 设置系统参数（es等工具需要）
#12）设置防火墙和selinux
#13）安装mysql，修改初始密码并建好cm相关用户

HEADER='\033[95m'
OKBLUE='\033[94m'
OKGREEN='\033[92m'
WARNING='\033[93m'
FAIL='\033[91m'
ENDC='\033[0m'


#set -x
unset PS4
export PS4='+{$LINENO:${FUNCNAME[0]}}'


function check_cmd_install() {
    local cmd_name=$1
    local tmp=`which $cmd_name`
    if [ $? -ne 0 ] ; then
        echo -e  $FAIL "\n************Installing $cmd_name error!************" $ENDC 
        exit 1
    fi

}



#安装mysql
function install_mysql()
{
<<EOF
    #安装mysql，修改初始密码
    echo -e "\n************install and set mysql************"  
    echo -e -n $OKGREEN "********stop mysql..." $ENDC
	#mysql5.6关闭方法
    service mysql stop 2>/dev/null
	#mysql5.7关闭方法
	service mysqld stop 2>/dev/null
	
	systemctl stop mysql
	systemctl stop mysqld
    #MysqlRpm=`rpm -qa | grep -i mysql` 


	MysqlRpm=`rpm -qa | grep -i mysql`
    echo "MysqlRpm:"$MysqlRpm	

    if [ -n $MysqlRpm ];then
        echo -e $OKGREEN "********uninstall old mysql..." $ENDC
        for sql in $MysqlRpm
        do
            rpm -e --nodeps $sql
        done
    fi
    
    rpm -e --nodeps `rpm -qa | grep -i maridb` 2>/dev/null 
    rpm -e --nodeps mariadb-libs 2>/dev/null
    rm -rf /var/lib/mysql
    rm -rf ~/.mysql_secret
	rm -rf /var/log/mysqld.log
	rm -rf /var/log/mysql.log

     
    echo -e -n $OKGREEN "install mysql..." $ENDC
EOF	

#	yum localinstall  -y  mysql-community-common
#	yum localinstall  -y mysql-community-libs
#	yum localinstall  -y mysql-community-libs-compat
#    yum localinstall  -y  mysql-community-client
#	yum localinstall  -y mysql-community-server
#	yum localinstall  -y  mysql-community-devel
	#yum install  -y mysql-community-minimal-debuginfo
    
<<EOF    
	yum localinstall  -y  mysql-community-common-5.7.24-1.el7.x86_64.rpm
	yum localinstall  -y mysql-community-libs-5.7.24-1.el7.x86_64.rpm
	yum localinstall  -y mysql-community-libs-compat-5.7.24-1.el7.x86_64.rpm
    yum localinstall  -y  mysql-community-client-5.7.24-1.el7.x86_64.rpm
    yum localinstall  -y mysql-community-server-5.7.24-1.el7.x86_64.rpm
    yum localinstall  -y  mysql-community-devel-5.7.24-1.el7.x86_64.rpm
	#yum localinstall  -y mysql-community-minimal-debuginfo
EOF

#	return 

	
#    check_cmd_install mysqld
#	service mysqld start 
 
	check_cmd_install mysql
	service mysql start
	
	if [ -f ~/.mysql_secret ];then 
		echo -e "get password from ~/.mysql_secret"
		mysql_passwd="`cat ~/.mysql_secret | awk '{print $NF}'`"  
	elif [ -f /var/log/mysqld.log ]; then
		echo -e "get password from /var/log/mysqld.log"
		pass_str=`grep 'temporary password' /var/log/mysqld.log|tail -1	`
		if [ "$pass_str" = "" ];then 
			echo -e $FAIL "can't get mysql password from /var/log/mysqld.log" $ENDC
			exit 1
		fi 
		mysql_passwd=`echo $pass_str|sed -n 's/.*[ |   ]*root@localhost:[ |   ]*\(.*\)[ |     ]*/\1/p'`
		
	else 
		echo -e $FAIL "can't find mysql password" $ENDC
		exit 1
	fi 
	       
    echo "mysql temporary password=$mysql_passwd"
	
	/usr/bin/expect << EOF 
    spawn mysql -uroot -p
	expect "Enter password"
	send "$mysql_passwd\r"
	expect "mysql>"
	send "set global validate_password_policy=0;set global validate_password_length=1;\r"
    expect "mysql>"
    send "set password=password('123456');\r"
    expect "mysql>"
    send "flush privileges;\r"
    expect "mysql>"
    send "quit\r"
    expect eof;
EOF
	
	
}




#安装mysql
install_mysql



