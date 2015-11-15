/*      NSEnergyFormatter.h
        Copyright (c) 2014, Apple Inc. All rights reserved.
 */

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, NSEnergyFormatterUnit) {
    NSEnergyFormatterUnitJoule = 11,
    NSEnergyFormatterUnitKilojoule = 14,
    NSEnergyFormatterUnitCalorie = (7 << 8) + 1,       // chemistry "calories", abbr "cal"
    NSEnergyFormatterUnitKilocalorie = (7 << 8) + 2,   // kilocalories in general, abbr “kcal”, or “C” in some locales (e.g. US) when usesFoodEnergy is set to YES
} NS_ENUM_AVAILABLE(10_10, 8_0);

NS_CLASS_AVAILABLE(10_10, 8_0)
@interface NSEnergyFormatter : NSFormatter {
@private
    void *_formatter;
    BOOL _isForFoodEnergyUse;
    void *_reserved[2];
}
@property (copy) NSNumberFormatter *numberFormatter;    // default is NSNumberFormatter with NSNumberFormatterDecimalStyle
@property NSFormattingUnitStyle unitStyle;              // default is NSFormattingUnitStyleMedium
@property (getter = isForFoodEnergyUse) BOOL forFoodEnergyUse; // default is NO; if it is set to YES, NSEnergyFormatterUnitKilocalorie may be “C” instead of “kcal"

// Format a combination of a number and an unit to a localized string.
- (NSString *)stringFromValue:(double)value unit:(NSEnergyFormatterUnit)unit;

// Format a number in joules to a localized string with the locale-appropriate unit and an appropriate scale (e.g. 10.3J = 2.46cal in the US locale).
- (NSString *)stringFromJoules:(double)numberInJoules;

// Return a localized string of the given unit, and if the unit is singular or plural is based on the given number.
- (NSString *)unitStringFromValue:(double)value unit:(NSEnergyFormatterUnit)unit;

// Return the locale-appropriate unit, the same unit used by -stringFromJoules:.
- (NSString *)unitStringFromJoules:(double)numberInJoules usedUnit:(NSEnergyFormatterUnit *)unitp;

// No parsing is supported. This method will return NO.
- (BOOL)getObjectValue:(out id *)obj forString:(NSString *)string errorDescription:(out NSString **)error;
@end