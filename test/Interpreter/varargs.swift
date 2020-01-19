// RUN: %target-run-simple-swift | %FileCheck %s

// REQUIRES: executable_test
// REQUIRES: objc_interop

// We don't have Foundation on baremetal.
// UNSUPPORTED: OS=none-eabi

import Foundation

func vf(_ params: CVarArg...) {
	print("OK")
}

var a: [AnyObject]! = ["a" as NSString]
var s: String! = "s"

vf(a as NSArray)
// CHECK: OK
vf(s as NSString)
// CHECK: OK
