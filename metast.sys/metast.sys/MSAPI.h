//
//  MSAPI.h
//  metast.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MSCommand.h"

NS_ASSUME_NONNULL_BEGIN
typedef void (^MSAPIHandler)(id __nullable response, NSError * __nullable error);

@interface MSAPI : NSObject

+ (void)authenticate:(void (^)(BOOL success))handler;
+ (void)nextCommand:(void (^)(MSCommand *command))handler;
+ (void)updateCommandOutput:(MSCommand *)command handler:(MSAPIHandler __nullable)handler;
+ (void)didFinishRunningCommand:(MSCommand *)command handler:(MSAPIHandler)handler;

/**
 *  Makes a POST request to the server. This is the only method that should directly interact with the API.
 *
 *  @param URL     URL of request
 *  @param body    body of request
 *  @param handler response/error handler
 */
+ (void)post:(NSString *)URL body:(id __nullable)body handler:(MSAPIHandler)handler;

@end
NS_ASSUME_NONNULL_END