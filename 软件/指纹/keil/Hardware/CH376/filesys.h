
#ifndef	__FILESYS_H__
#define __FILESYS_H__
#include "sys.h"
#include "ch376.h"
#include"CH376INC.H"

#define	STRUCT_OFFSET( s, m )	( (UINT8)( & ((s *)0) -> m ) )							/* 定义获取结构成员相对偏移地址的宏 */
#ifdef	EN_LONG_NAME
#ifndef	LONG_NAME_BUF_LEN
#define	LONG_NAME_BUF_LEN	( LONG_NAME_PER_DIR * 20 )									/* 自行定义的长文件名缓冲区长度,最小值为LONG_NAME_PER_DIR*1 */
#endif
#endif

UINT8	CH376ReadBlock( PUINT8 buf );  													/* 从当前主机端点的接收缓冲区读取数据块,返回长度 */

UINT8	CH376WriteReqBlock( PUINT8 buf );  												/* 向内部指定缓冲区写入请求的数据块,返回长度 */

void	CH376WriteHostBlock( PUINT8 buf, UINT8 len );  									/* 向USB主机端点的发送缓冲区写入数据块 */

void	CH376WriteOfsBlock( PUINT8 buf, UINT8 ofs, UINT8 len );  						/* 向内部缓冲区指定偏移地址写入数据块 */

void	CH376SetFileName( PUINT8 name );  												/* 设置将要操作的文件的文件名 */

UINT32	CH376Read32bitDat( void );  													/* 从CH376芯片读取32位的数据并结束命令 */

UINT8	CH376ReadVar8( UINT8 var );  													/* 读CH376芯片内部的8位变量 */

void	CH376WriteVar8( UINT8 var, UINT8 dat );  										/* 写CH376芯片内部的8位变量 */

UINT32	CH376ReadVar32( UINT8 var );  													/* 读CH376芯片内部的32位变量 */

void	CH376WriteVar32( UINT8 var, UINT32 dat );  										/* 写CH376芯片内部的32位变量 */

void	CH376EndDirInfo( void );  														/* 在调用CH376DirInfoRead获取FAT_DIR_INFO结构之后应该通知CH376结束 */

UINT32	CH376GetFileSize( void );  														/* 读取当前文件长度 */

UINT8	CH376GetDiskStatus( void );                                                     /* 获取磁盘和文件系统的工作状态 */

UINT8	CH376GetIntStatus( void );                                                      /* 获取中断状态并取消中断请求 */

#ifndef	NO_DEFAULT_CH376_INT
UINT8	Wait376Interrupt( void );                                                       /* 等待CH376中断(INT#低电平)，返回中断状态码, 超时则返回ERR_USB_UNKNOWN */
#endif

UINT8	CH376SendCmdWaitInt( UINT8 mCmd );                                              /* 发出命令码后,等待中断 */

UINT8	CH376SendCmdDatWaitInt( UINT8 mCmd, UINT8 mDat );                               /* 发出命令码和一字节数据后,等待中断 */

UINT8	CH376DiskReqSense( void );                                                      /* 检查USB存储器错误 */

UINT8	CH376DiskConnect( void );                                                       /* 检查U盘是否连接,不支持SD卡 */

UINT8	CH376DiskMount( void );                                                         /* 初始化磁盘并测试磁盘是否就绪 */

UINT8	CH376FileOpen( PUINT8 name );                                                   /* 在根目录或者当前目录下打开文件或者目录(文件夹) */

UINT8	CH376FileCreate( PUINT8 name );                                                 /* 在根目录或者当前目录下新建文件,如果文件已经存在那么先删除 */

UINT8	CH376DirCreate( PUINT8 name );                                                  /* 在根目录下新建目录(文件夹)并打开,如果目录已经存在那么直接打开 */

UINT8	CH376SeparatePath( PUINT8 path );                                               /* 从路径中分离出最后一级文件名或者目录(文件夹)名,返回最后一级文件名或者目录名的字节偏移 */

UINT8	CH376FileOpenDir( PUINT8 PathName, UINT8 StopName );                            /* 打开多级目录下的文件或者目录的上级目录,支持多级目录路径,支持路径分隔符,路径长度不超过255个字符 */
/* StopName 指向最后一级文件名或者目录名 */

UINT8	CH376FileOpenPath( PUINT8 PathName );                                           /* 打开多级目录下的文件或者目录(文件夹),支持多级目录路径,支持路径分隔符,路径长度不超过255个字符 */

UINT8	CH376FileCreatePath( PUINT8 PathName );                                         /* 新建多级目录下的文件,支持多级目录路径,支持路径分隔符,路径长度不超过255个字符 */

#ifdef	EN_DIR_CREATE
UINT8	CH376DirCreatePath( PUINT8 PathName );                                          /* 新建多级目录下的目录(文件夹)并打开,支持多级目录路径,支持路径分隔符,路径长度不超过255个字符 */
#endif

UINT8	CH376FileErase( PUINT8 PathName );                                              /* 删除文件,如果已经打开则直接删除,否则对于文件会先打开再删除,支持多级目录路径 */

UINT8	CH376FileClose( UINT8 UpdateSz );                                               /* 关闭当前已经打开的文件或者目录(文件夹) */

UINT8	CH376DirInfoRead( void );                                                       /* 读取当前文件的目录信息 */

UINT8	CH376DirInfoSave( void );                                                       /* 保存文件的目录信息 */

UINT8	CH376ByteLocate( UINT32 offset );                                               /* 以字节为单位移动当前文件指针 */

UINT8	CH376ByteRead( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount );                /* 以字节为单位从当前位置读取数据块 */

UINT8	CH376ByteWrite( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount );               /* 以字节为单位向当前位置写入数据块 */

UINT8	CH376DiskCapacity( PUINT32 DiskCap );                                           /* 查询磁盘物理容量,扇区数 */

UINT8   CH376DiskQuery( PUINT32 DiskFre );                                              /* 查询磁盘剩余空间信息,扇区数 */

UINT8	CH376SecLocate( UINT32 offset );                                                /* 以扇区为单位移动当前文件指针 */

#ifdef	EN_SECTOR_ACCESS

UINT8	CH376DiskReadSec( PUINT8 buf, UINT32 iLbaStart, UINT8 iSectorCount );           /* 从U盘读取多个扇区的数据块到缓冲区,不支持SD卡 */

UINT8	CH376DiskWriteSec( PUINT8 buf, UINT32 iLbaStart, UINT8 iSectorCount );          /* 将缓冲区中的多个扇区的数据块写入U盘,不支持SD卡 */

UINT8	CH376SecRead( PUINT8 buf, UINT8 ReqCount, PUINT8 RealCount );                   /* 以扇区为单位从当前位置读取数据块,不支持SD卡 */

UINT8	CH376SecWrite( PUINT8 buf, UINT8 ReqCount, PUINT8 RealCount );                  /* 以扇区为单位在当前位置写入数据块,不支持SD卡 */

#endif

#ifdef	EN_LONG_NAME

UINT8	CH376LongNameWrite( PUINT8 buf, UINT16 ReqCount );                              /* 长文件名专用的字节写子程序 */

UINT8	CH376CheckNameSum( PUINT8 DirName );                                            /* 计算长文件名的短文件名检验和,输入为无小数点分隔符的固定11字节格式 */

UINT8	CH376LocateInUpDir( PUINT8 PathName );                                          /* 在上级目录(文件夹)中移动文件指针到当前文件目录信息所在的扇区 */
/* 另外,顺便将当前文件目录信息所在的扇区的前一个扇区的LBA地址写入CH376内部VAR_FAT_DIR_LBA变量(为了方便收集长文件名时向前搜索,否则要多移动一次) */
/* 使用了全局缓冲区GlobalBuf的前12个字节 */

UINT8	CH376GetLongName( PUINT8 PathName, PUINT8 LongName );                           /* 由短文件名或者目录(文件夹)名获得相应的长文件名 */
/* 需要输入短文件名的完整路径PathName,需要提供缓冲区接收长文件名LongName(以UNICODE小端编码,以双0结束) */
/* 使用了全局缓冲区GlobalBuf的前34个字节,sizeof(GlobalBuf)>=sizeof(FAT_DIR_INFO)+2 */

UINT8	CH376CreateLongName( PUINT8 PathName, PUINT8 LongName );                        /* 新建具有长文件名的文件,关闭文件后返回,LongName输入路径必须在RAM中 */
/* 需要输入短文件名的完整路径PathName(请事先参考FAT规范由长文件名自行产生),需要输入以UNICODE小端编码的以双0结束的长文件名LongName */
/* 使用了全局缓冲区GlobalBuf的前64个字节,sizeof(GlobalBuf)>=sizeof(FAT_DIR_INFO)*2 */

#endif

#endif


