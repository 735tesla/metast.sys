//
//  MSAPI.h
//  metast.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN
@interface MSAPI : NSObject

/**
 *  Makes a POST request to the server. This is the only method that should directly interact with the API.
 *
 *  @param URL     URL of request
 *  @param body    body of request
 *  @param handler response/error handler
 */
+ (void)post:(NSString *)URL body:(id __nullable)body handler:(void (^)(id __nullable response, NSError * __nullable error))handler;

@end
NS_ASSUME_NONNULL_END