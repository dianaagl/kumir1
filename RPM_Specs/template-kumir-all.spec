# norootforbuild

# THIS FILE IS TEMPLATE, NOT A SPEC !!!
#
# Replace:
# --REVISION--		to revision number
# --REQUIRES--		to qt libs of target distro (>= 4.6.0) and other distro specific packages
# --BUILD_REQUIRES--	to qt development of target distro (>= 4.6.0) and other distro specific packages
# --PRE_REQ--		distro specific pre-requirements
# --PACKAGER--		packager name and email

# Examples for openSUSE 11.0/11.1/11.2/Factory:
# --REQUIRES--		libqt4 >= 4.6.0 libqt4-x11 >= 4.6.0 libQtWebKit4 >= 4.6.0
# --BUILD_REQUIRES--	python libqt4-devel >= 4.6.0 libQtWebKit-devel >= 4.6.0 -post-build-checks -rpmlint-Factory
# --PRE_REQ--		shared-mime-info

%define svn_rev --REVISION--



Name:		kumir-all
Summary:	KUMIR education system
License:	GPL
Group:		Education
Version:	1.7.90+svn%{svn_rev}
Release:	0
BuildRoot:	%{_tmppath}/kumir-%{version}
BuildRequires:	--BUILD_REQUIRES--
Requires:	--REQUIRES--
PreReq:		--PRE_REQ--
Vendor:		NIISI RAS
Source:		kumir-%{version}.tar.bz2
Packager:	--PACKAGER--
URL:		http://lpm.org.ru/kumir/

%description
Complete KUMIR education system

%description -l ru_RU.UTF-8
Комплект Учебных МИРов

%prep
%setup -q -n kumir-1.7.90

%build
python ./configure --prefix=$RPM_BUILD_ROOT/usr --target-dir=$RPM_BUILD_ROOT/opt/kumir
make
strip -s kumir
strip -s pictomir
strip -s pluginstarter

%install
make install

%clean
#rm -rf $RPM_BUILD_ROOT

%package -n kumir-common-libs
Summary:    Shared libraries for Kumir and Pictomir
Summary(ru_RU.UTF-8): Библиотеки среды КуМир
Group:		Education

%description -n kumir-common-libs
Shared libraries for Kumir and Pictomir:
  - Interpreter runtime
  - Python subprocess wrapper

%description -n kumir-common-libs -l ru_RU.UTF-8
Разделяемые библиотеки системы КуМир:
  - Интерпретатор
  - Оболочка процесса Python
  
%files -n kumir-common-libs
%defattr(-,root,root)
/opt/kumir/Libraries/libkrt.so
/opt/kumir/Libraries/libkpython.so
/opt/kumir/Libraries/libInteractiveSVG.so

%package -n kumir
Summary:	Kumir Language Implementation (development version)
Summary(ru_RU.UTF-8): Реализация системы КуМир (текущая версия)
Group:		Education
Requires:   kumir-common-libs >= %{version}

%description -n kumir
Implementation of Kumir programming language, designed by academic Ershov.
Includes compiler, runtime, IDE, Robot and Draw.

%description -n kumir -l ru_RU.UTF-8
Реализация системы КУМИР в соответствии с учебником Анатолия Георгиевича Кушниренко.
Включает в себя среду программирования на русском алгоритмическом языке а также
канонические исполнители: Робот и Чертёжник.

%post -n kumir
cd /usr/share/mime
rm -f XMLnamespaces aliases globs magic subclasses
update-mime-database /usr/share/mime > /dev/null

%postun -n kumir
cd /usr/share/mime
rm -f XMLnamespaces aliases globs magic subclasses application/x-kumir-program.xml
update-mime-database /usr/share/mime > /dev/null

%files -n kumir
%defattr(-,root,root)
/opt/kumir/Kumir/*
/opt/kumir/kumir
/usr/bin/kumir
/usr/share/applications/kumir.desktop
/usr/share/pixmaps/kumir.png

%package -n pictomir
Summary:	Pixmap based programming system for children
Summary(ru_RU.UTF-8): Система программирования для самых маленьких
Requires:	kumir-worlds-isometricrobot >= %{version}
Group:		Education
Requires:   kumir-common-libs >= %{version}

%description -n pictomir
Education system based on pixmap-to-field method.

%description -n pictomir -l ru_RU.UTF-8
Система обучения алгоритмическим навыкам для детей с раннего возвраста



%files -n pictomir
%defattr(-,root,root)
/opt/kumir/Pictomir/*
/opt/kumir/pictomir
/usr/bin/pictomir
/usr/share/applications/pictomir.desktop
/usr/share/pixmaps/pictomir.png

%package -n kumir-pluginstarter
Summary:	Starter to use Kumir Worlds without Kumir
Summary(ru_RU.UTF-8): Система запуска Миров без помощи Кумира
Group:		Education

%description -n kumir-pluginstarter
Starter to use Kumir Worlds without Kumir

%description -n kumir-pluginstarter -l ru_RU.UTF-8
Система запуска Миров без помощи Кумира

%files -n kumir-pluginstarter
%defattr(-,root,root)
/opt/kumir/pluginstarter
/usr/bin/kumpluginstarter

%package -n kumir-convertor
Summary:	Convertor from Kumir language to C99 and C++
Summary(ru_RU.UTF-8): Конвертор из языка Кумир в языки C99 и C++
Requires:	kumir >= %{version} python >= 2.6.0
Group:		Education

%description -n kumir-convertor
Convertor from Kumir language to C99 and C++

%description -n kumir-convertor -l ru_RU.UTF-8
Конвертор из языка Кумир в языки C99 и C++

%files -n kumir-convertor
%defattr(-,root,root)
/opt/kumir/Addons/c99.convertor
/opt/kumir/Addons/c_plus_plus.convertor
/opt/kumir/Addons/convertor/*

%package -n kumir-worlds-isometricrobot
Summary:	Isometric robot for Kumir and Pictomir
Summary(ru_RU.UTF-8): Робот-Вертун для Кумира и Пиктомира
Requires:	kumir-pluginstarter >= %{version}
Group:		Education

%description -n kumir-worlds-isometricrobot
Isometric robot for Kumir and Pictomir

%description -n kumir-worlds-isometricrobot -l ru_RU.UTF-8
Робот-Вертун для Кумира и Пиктомира

%files -n kumir-worlds-isometricrobot
%defattr(-,root,root)
/opt/kumir/Addons/libisometricRobot.so
/opt/kumir/Addons/isometricRobot/resources/*

%package -n kumir-worlds-turtle
Summary:	Tutle for Kumir and Pictomir
Summary(ru_RU.UTF-8): Черепаха для Кумира и Пиктомира
Requires:	kumir-pluginstarter >= %{version}
Group:		Education

%description -n kumir-worlds-turtle
Turtle for Kumir anf Pictomir

%description -n kumir-worlds-turtle -l ru_RU.UTF-8
Черепаха для Кумира и Пиктомира

%files -n kumir-worlds-turtle
%defattr(-,root,root)
/opt/kumir/Addons/libturtle.so

%package -n kumir-worlds-dwunog
Summary:	Bipod for Kumir and Pictomir
Summary(ru_RU.UTF-8): Двуног для Кумира и Пиктомира
Requires:	kumir-pluginstarter >= %{version}
Group:		Education

%description -n kumir-worlds-dwunog
Bipod for Kumir anf Pictomir

%description -n kumir-worlds-dwunog -l ru_RU.UTF-8
Двуног для Кумира и Пиктомира

%files -n kumir-worlds-dwunog
%defattr(-,root,root)
/opt/kumir/Addons/libdwunog.so
/opt/kumir/Addons/dwunog/*

%package -n kumir-worlds-kuznechik
Summary:	Grasshopper for Kumir and Pictomir
Summary(ru_RU.UTF-8): Кузнечик для Кумира и Пиктомира
Requires:	kumir-pluginstarter >= %{version}
Group:		Education

%description -n kumir-worlds-kuznechik
Grasshopper for Kumir and Pictomir

%description -n kumir-worlds-kuznechik -l ru_RU.UTF-8
Кузнечик для Кумира и Пиктомира

%files -n kumir-worlds-kuznechik
%defattr(-,root,root)
/opt/kumir/Addons/libkuznechik.so

%package -n kumir-worlds-vodoley
Summary:	Aquarius for Kumir and Pictomir
Summary(ru_RU.UTF-8): Водолей для Кумира и Пиктомира
Requires:	kumir-pluginstarter >= %{version}
Group:		Education

%description -n kumir-worlds-vodoley
Aquarius for Kumir anf Pictomir

%description -n kumir-worlds-vodoley -l ru_RU.UTF-8
Водолей для Кумира и Пиктомира

%files -n kumir-worlds-vodoley
%defattr(-,root,root)
/opt/kumir/Addons/libvodoley.so
