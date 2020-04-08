import Foundation

let PORT_NUMBER = 2345

extension String {
    
    /** A representation of the string in DispatchData form */
    var dispatchData: DispatchData {
        let data = self.data(using: .utf8)!
        let dispatchData = data.withUnsafeBytes {
            DispatchData(bytes: UnsafeBufferPointer(start: $0, count: data.count))
        }
        
        return dispatchData
    }
    
}

extension DispatchData {
    
    /** Converts DispatchData back into a String format */
    func toString() -> String {
        return String(bytes: self, encoding: .utf8)!
    }
    
    /** Converts DispatchData back into a Dictionary format */
    func toDictionary() -> NSDictionary {
        return NSDictionary.init(contentsOfDispatchData: self as __DispatchData)
    }
    
}

/** The different types of data to be used with Peertalk */
enum PTType: UInt32 {
    case PTExampleFrameTypeDeviceInfo = 100
    case PTExampleFrameTypeTextMessage = 101
    case PTExampleFrameTypePing = 102
    case PTExampleFrameTypePong = 103
    case PTFacePRSData = 104
    case PTFaceExpressionData = 105
    case PTFaceHasArrived = 106
    case PTFaceHasLeft = 107
}
