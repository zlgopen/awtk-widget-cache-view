# awtk-widget-cache_view

cache_view 控件。将全部子控件绘制到一个 bitmap 中缓存起来，真正绘制时直接贴图，以提高绘制效率。

## 介绍

有时窗口上有很多控件，其中一部分的内容更新比较慢 (>100ms)，另外一部分则需要实时刷新（如动画）。

在不支持脏矩形或脏矩形区域比较大的时候，我们可以把内容更新比较慢的控件放到 cache\_view 控件中，作为了 cache\_view 的子控件，cache\_view 把所有子控件绘制到一张 bitmap 中，并定期更新这个 bitmap。在每次渲染时 cache\_view 直接把 bitmap 上去，可大大提供渲染效率。

cache\_view 会尽量保持 bitmap 的格式与 lcd 的格式一致，以便使用 memcpy 进行贴图，以达到最高效率。

## 使用方法：

把经常更新的控件放到 cache\_view 中，并给 cache\_view 指定更新时间间隔 update_interval（毫秒）。

cache\_view 的大小一般指定为窗口一样大，并把窗口的背景设置为 cache\_view 的背景，窗口不再指定背景，避免重复绘制。

如：

```xml
<window theme="main">
	<cache_view x="0" y="0" w="100%" h="100%" update_interval="300">
		<image x="0" y="0" w="100%" h="50%" image="logo" rotation="0" />
		<image x="0" y="0" w="100%" h="50%" image="logo" rotation="1" />
		<image x="0" y="0" w="100%" h="50%" image="logo" rotation="2" />
		<image x="0" y="0" w="100%" h="50%" image="logo" rotation="3" />
		<image x="0" y="0" w="100%" h="50%" image="logo" rotation="4" />
		<image x="0" y="0" w="100%" h="50%" image="logo" rotation="5" />
		<digit_clock x="c" y="m:30" w="80" h="30" format="YY/MM/DD h:mm:ss" />
		<check_button x="10" y="b:30" w="80" h="30" name="disable" text="Disable Cache" />
		<button x="c:60" y="b:30" w="80" h="30" name="close" text="Close" />
	</cache_view>
  
	<guage x="c" y="10" w="240" h="240" >
		<guage_pointer x="c" y="50" w="24" h="140" value="-128" image="guage_pointer" animation="value(from=-128, to=128, yoyo_times=6, duration=3000, delay=1000, auto_destroy=false)" />
	</guage>
	<guage x="c" y="bottom:60" w="240" h="240" >
		<guage_pointer x="c" y="50" w="12" h="140" value="-128" image="pointer" animation="value(from=-128, to=128, yoyo_times=1000, duration=3000)" />
	</guage>
</window>

```

## 局限

* 目前不支持高清屏。
* 缓存图片需要更多内存。

## 准备

1. 获取 awtk 并编译

```
git clone https://github.com/zlgopen/awtk.git
cd awtk; scons; cd -
```

2. 获取 awtk-widget-cache-view 并编译

```
https://github.com/zlgopen/awtk-widget-cache-view.git
cd awtk-widget-cache-view; scons; 
```

## 运行

> 需要把 awtk/bin 下的动态库拷贝到当前目录的 bin 中。

```
./bin/demo_cache_view
```

## 文档

[AWTK 自定义控件规范](https://github.com/zlgopen/awtk/blob/master/docs/custom_widget_rules.md)
