//
//  macOSUtil.m
//  TsoEngine
//
//  Created by SuchanTso on 2024/1/12.
//
#ifdef TSO_PLATFORM_MACOSX
#import <Foundation/Foundation.h>
#include <Cocoa/Cocoa.h>
#include "Tso/Utils/PlatformUtils.h"
#include <string>
#include "GLFW/glfw3.h"
#include "Application.h"

// Objective-C++代码段
namespace Tso{
static std::string ConvertWindowsFilterToStr(const std::string& filter){
    auto dotPos = filter.find('.');
    auto hemiParenthesis = filter.find(')');
    if(dotPos == std::string::npos || hemiParenthesis == std::string::npos){
        return "";
    }
    int count = hemiParenthesis - dotPos;
    return filter.substr(dotPos + 1,count - 1);
}


extern "C" std::string FileDialogs::OpenFile(const char* filter) {
    __block std::string ret;
    
    std::string extType = ConvertWindowsFilterToStr(filter);
    dispatch_block_t run = ^{
      NSOpenPanel *panel = [NSOpenPanel openPanel];
      NSString *file = [NSString stringWithCString:extType.c_str() encoding:[NSString defaultCStringEncoding]];
      NSArray* fileTypes = [file componentsSeparatedByString:@" "];
      [panel setMessage:@"select a working path"];
      [panel setPrompt:@"OK"];
      [panel setCanChooseDirectories:NO];
      [panel setCanCreateDirectories:NO];
      [panel setCanChooseFiles:YES];
      [panel setAllowsMultipleSelection:NO];
      [panel setAllowedFileTypes:fileTypes];

      NSString *path_all=@"";
      NSArray *select_files;
      NSInteger result = [panel runModal];
      //m_LB1.stringValue =@"";
      if (result == NSFileHandlingPanelOKButton)
      {
          select_files = [panel filenames] ;
          for (int i=0; i<select_files.count; i++)
          {
              path_all= [select_files objectAtIndex:i];
              ret = [path_all UTF8String];
          }
      }
    };
    if (NSThread.isMainThread) {
        run();
    } else {
        dispatch_sync(dispatch_get_main_queue(), run);
    }
    return ret;
    
}

extern "C" std::string FileDialogs::SaveFile(const char* filter) {
    __block std::string ret;
    std::string extType = ConvertWindowsFilterToStr(filter);
    
    dispatch_block_t run = ^{
        NSSavePanel *savePanel = [NSSavePanel savePanel];
//        NSString *file = [NSString stringWithCString:extType.c_str() encoding:[NSString defaultCStringEncoding]];
        
        [savePanel setCanCreateDirectories:YES];
        [savePanel setAllowedContentTypes:@[]]; // 空数组表示只允许选择目录
        
        NSInteger result = [savePanel runModal];
            if (result == NSModalResponseOK) {
                NSURL* url = [savePanel URL];
                if(url != nil){
                    CFStringRef cfstr = (__bridge CFStringRef)[url absoluteString];
                    const char *cStr = CFStringGetCStringPtr(cfstr, kCFStringEncodingUTF8);
                    ret = cStr;
                }
            }
        
    };
    if (NSThread.isMainThread) {
        run();
    } else {
        dispatch_sync(dispatch_get_main_queue(), run);
    }
    std::string temRes = ret + "." +extType;
    return temRes.substr(7);//remove "file://"
}

}
#endif
