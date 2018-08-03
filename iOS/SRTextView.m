//
//  SRUITextView.m
//  Kindle App
//
//  Created by Xian, Ying on 3/21/18.
//  Copyright © 2018 Amazon.com. All rights reserved.
//

#import "SearchStrings+Constants.h"
#import "LoggingManager.h"
#import "SRTextView.h"

@implementation SRTextView

NSMutableAttributedString *_attributedEllipsis;

- (void)awakeFromNib
{
    [super awakeFromNib];
    [self initialSetup];
}

- (void)initialSetup
{
    self.editable = NO;
    self.selectable = NO;
    self.textContainerInset = UIEdgeInsetsZero;
    self.textContainer.lineFragmentPadding = 0;
    self.textContainer.maximumNumberOfLines = 3;
    self.textContainer.lineBreakMode = NSLineBreakByTruncatingTail;
    _attributedEllipsis = [[NSMutableAttributedString alloc]initWithString:BookSummaryAggregator_Ellipsis];
    [_attributedEllipsis addAttribute:NSFontAttributeName value:self.font range:NSMakeRange(0,_attributedEllipsis.length)];
}

- (void)setAttributedText:(NSAttributedString *)attributedText
{
    [self.textStorage setAttributedString:attributedText];
    [self.textContainer setSize:CGSizeMake(self.bounds.size.width, CGFLOAT_MAX)];
    NSRange glyphRange = [self.layoutManager glyphRangeForTextContainer:self.textContainer];
    KINDLE_LOG_DEBUG(@"glyphRangeForTextContainer:%@", NSStringFromRange(glyphRange));

    NSInteger lastGlyphIndex = NSMaxRange(glyphRange) - 1;
    NSRange truncatedGlyphRange = [self.layoutManager truncatedGlyphRangeInLineFragmentForGlyphAtIndex:lastGlyphIndex];
    KINDLE_LOG_DEBUG(@"truncatedGlyphRangeInLineFragmentForGlyphAtIndex:%@", NSStringFromRange(truncatedGlyphRange));

    if (truncatedGlyphRange.location != NSNotFound)
    {
        NSInteger lastVisibleCharacterIndex = [self.layoutManager characterIndexForGlyphAtIndex:truncatedGlyphRange.location] - 1;
        KINDLE_LOG_DEBUG(@"lastVisibleCharacterIndex:%ld", (long)lastVisibleCharacterIndex);

        NSRange characterRangeToReplace = NSMakeRange(lastVisibleCharacterIndex, self.attributedText.length - lastVisibleCharacterIndex);
        [self.textStorage replaceCharactersInRange:characterRangeToReplace withAttributedString:_attributedEllipsis];
        KINDLE_LOG_DEBUG(@"attributed string after replace:%@", self.attributedText.string);
    }
}

- (void)setTextAlignment:(NSTextAlignment)textAlignment
{
    [super setTextAlignment:textAlignment];

    if (textAlignment == NSTextAlignmentRight)
    {
        [self.textStorage addAttribute:NSWritingDirectionAttributeName  value:@[@(NSWritingDirectionRightToLeft | NSWritingDirectionEmbedding)] range:NSMakeRange(0, self.attributedText.length)];
    }
}

@end
