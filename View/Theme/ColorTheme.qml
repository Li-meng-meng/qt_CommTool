import QtQuick 2.15
pragma Singleton

QtObject {
    property bool isDark: false

    // ========== 亮色主题 (Light) ==========
    property color light_bgMain:           "#FFFFFF"
    property color light_border:           "#E5E7EB"
    property color light_bgInner:          "#F3F4F6"
    property color light_bgChart:          "#F9FAFB"
    property color light_btnPrimary:       "#2563EB"
    property color light_btnPrimaryHover:  "#1D4ED8"
    property color light_btnPrimaryPressed:"#1E40AF"
    property color light_btnPrimaryText:   "#FFFFFF"
    property color light_btnNormal:        "#1d4fd879"
    property color light_btnNormalHover:   "#F3F4F6"
    property color light_btnNormalText:    "#111827"
    property color light_btnNormalBorder:  "#E5E7EB"
    property color light_textMain:         "#111827"
    property color light_textSub:          "#6B7280"
    property color light_textSuccess:      "#16A34A"
    property color light_textDisabled:     "#64748B"
    property color light_textWhite:        "#FFFFFF"
    property color light_btnDanger:        "#DC2626"
    property color light_btnDangerHover:   "#B91C1C"
    property color light_btnDangerPressed: "#991B1B"
    property color light_btnDangerText:    "#FFFFFF"
    property color light_btnSuccess:       "#16A34A"
    property color light_btnSuccessHover:  "#15803D"
    property color light_btnSuccessPressed:"#166534"
    property color light_btnSuccessText:   "#FFFFFF"
    property color light_toastSuccess:     "#16A34A"
    property color light_toastError:       "#DC2626"
    property color light_toastInfo:        "#1E293B"
    property color light_toastText:        "#FFFFFF"

    // ========== 暗黑主题 (Dark) ==========
    property color dark_bgMain:            "#121212"
    property color dark_border:            "#374151"
    property color dark_bgInner:           "#1F2937"
    property color dark_bgChart:           "#1E1E2E"
    property color dark_btnPrimary:        "#3B82F6"
    property color dark_btnPrimaryHover:   "#60A5FA"
    property color dark_btnPrimaryPressed: "#1D4ED8"
    property color dark_btnPrimaryText:    "#FFFFFF"
    property color dark_btnNormal:         "#1F2937"
    property color dark_btnNormalHover:    "#374151"
    property color dark_btnNormalText:     "#F9FAFB"
    property color dark_btnNormalBorder:   "#374151"
    property color dark_textMain:          "#F9FAFB"
    property color dark_textSub:           "#9CA3AF"
    property color dark_textSuccess:       "#16A34A"
    property color dark_textDisabled:      "#64748B"
    property color dark_textWhite:         "#FFFFFF"
    property color dark_btnDanger:         "#EF4444"
    property color dark_btnDangerHover:    "#F87171"
    property color dark_btnDangerPressed:  "#DC2626"
    property color dark_btnDangerText:     "#FFFFFF"
    property color dark_btnSuccess:        "#22C55E"
    property color dark_btnSuccessHover:   "#4ADE80"
    property color dark_btnSuccessPressed: "#16A34A"
    property color dark_btnSuccessText:    "#FFFFFF"
    property color dark_toastSuccess:      "#16A34A"
    property color dark_toastError:        "#DC2626"
    property color dark_toastInfo:         "#1E293B"
    property color dark_toastText:         "#FFFFFF"

    // ========== 动态适配 ==========
    property color bgMain:           isDark ? dark_bgMain : light_bgMain
    property color border:           isDark ? dark_border : light_border
    property color bgInner:          isDark ? dark_bgInner : light_bgInner
    property color bgChart:          isDark ? dark_bgChart : light_bgChart
    property color btnPrimary:       isDark ? dark_btnPrimary : light_btnPrimary
    property color btnPrimaryHover:  isDark ? dark_btnPrimaryHover : light_btnPrimaryHover
    property color btnPrimaryPressed:isDark ? dark_btnPrimaryPressed : light_btnPrimaryPressed
    property color btnPrimaryText:   isDark ? dark_btnPrimaryText : light_btnPrimaryText
    property color btnNormal:        isDark ? dark_btnNormal : light_btnNormal
    property color btnNormalHover:   isDark ? dark_btnNormalHover : light_btnNormalHover
    property color btnNormalText:    isDark ? dark_btnNormalText : light_btnNormalText
    property color btnNormalBorder:  isDark ? dark_btnNormalBorder : light_btnNormalBorder
    property color textMain:         isDark ? dark_textMain : light_textMain
    property color textSub:          isDark ? dark_textSub : light_textSub
    property color textSuccess:      isDark ? dark_textSuccess : light_textSuccess
    property color textDisabled:     isDark ? dark_textDisabled : light_textDisabled
    property color textWhite:        isDark ? dark_textWhite : light_textWhite
    property color btnDanger:        isDark ? dark_btnDanger : light_btnDanger
    property color btnDangerHover:   isDark ? dark_btnDangerHover : light_btnDangerHover
    property color btnDangerPressed: isDark ? dark_btnDangerPressed : light_btnDangerPressed
    property color btnDangerText:    isDark ? dark_btnDangerText : light_btnDangerText
    property color btnSuccess:       isDark ? dark_btnSuccess : light_btnSuccess
    property color btnSuccessHover:  isDark ? dark_btnSuccessHover : light_btnSuccessHover
    property color btnSuccessPressed:isDark ? dark_btnSuccessPressed : light_btnSuccessPressed
    property color btnSuccessText:   isDark ? dark_btnSuccessText : light_btnSuccessText
    property color toastSuccess:     isDark ? dark_toastSuccess : light_toastSuccess
    property color toastError:       isDark ? dark_toastError : light_toastError
    property color toastInfo:        isDark ? dark_toastInfo : light_toastInfo
    property color toastText:        isDark ? dark_toastText : light_toastText
}
