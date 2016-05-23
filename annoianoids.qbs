import qbs
import qbs.Process
import qbs.File
import qbs.FileInfo
import qbs.TextFile
import "../../../libs/openFrameworksCompiled/project/qtcreator/ofApp.qbs" as ofApp

Project{
    property string of_root: '../../..'

    ofApp {
        name: { return FileInfo.baseName(path) }

        files: [
            'src/main.cpp',
            'src/ofApp.cpp',
            'src/ofApp.h',
            'src/KinectBlobTracker.h',
            'src/KinectBlobTracker.cpp',
            'src/ofxGuiImage.h',
            'src/ofxGuiImage.cpp',
            'src/ofxFpsSlider.h',
            'src/ofxFpsSlider.cpp',
            'src/background.h',
            'src/photon.h',
            'src/sprite_image.h',
            'src/asteroid.h',
            'src/stuff.h',
            'src/world.h',
            'src/sprite.h',
            'src/polygon.h',
            'src/outline.h',

        ]

        // This project is using addons.make to include the addons
        // since it was imported from old code. To change it to include
        // the addons from the qbs file change the following lines to
        // the list of used addons in array format. eg:
        //
        of.addons: [
            "ofxGui",
            "ofxKinect",
            "ofxXmlSettings",
            "ofxOpenCv",
            "ofxGamepad",
            "ofxBox2d",
            "ofxGif",
        ]

        // additional flags for the project. the of module sets some
        // flags by default to add the core libraries, search paths...
        // this flags can be augmented through the following properties:
        of.pkgConfigs: []       // list of additional system pkgs to include
        of.includePaths: [
            "/home/mda/Lab/of_v0.9.0_linux64_release/addons/ofxBox2d/libs",
            "/home/mda/Lab/of_v0.9.0_linux64_release/addons/ofxGamePad/libs/wgois/include"
        ];
        of.cFlags: []           // flags passed to the c compiler
        of.cxxFlags: []         // flags passed to the c++ compiler
        of.linkerFlags: [
            '/home/mda/Lab/of_v0.9.0_linux64_release/addons/ofxGamepad/libs/wgois/lib/linux64/libwgois64.a',
        ]      // flags passed to the linker
        of.defines: []          // defines are passed as -D to the compiler
        // and can be checked with #ifdef or #if in the code
        of.frameworks: []       // osx only, additional frameworks to link with the project

        // other flags can be set through the cpp module: http://doc.qt.io/qbs/cpp-module.html
        // eg: this will enable ccache when compiling
        //
        // cpp.compilerWrapper: 'ccache'

        Depends{
            name: "cpp"
        }

        // common rules that parse the include search paths, core libraries...
        Depends{
            name: "of"
        }

        // dependency with the OF library
        Depends{
            name: "openFrameworks"
        }
    }

    references: [FileInfo.joinPaths(of_root, "/libs/openFrameworksCompiled/project/qtcreator/openFrameworks.qbs")]
}
