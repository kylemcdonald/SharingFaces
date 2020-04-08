import Foundation

// MARK: - Delegate
protocol PTManagerDelegate {
    
    /** Return whether or not you want to accept the specified data type */
    func peertalk(shouldAcceptDataOfType type: UInt32) -> Bool
    
    /** Runs when the device has received data */
    func peertalk(didReceiveData data: Data, ofType type: UInt32, withTag tag: UInt32)
    
    /** Runs when the connection has changed */
    func peertalk(didChangeConnection connected: Bool)
    
}
// MARK: - iOS

class PTManager: NSObject {
    
    static let instance = PTManager()
    
    // MARK: Properties
    var delegate: PTManagerDelegate?
    var portNumber: Int?
    weak var serverChannel: PTChannel?
    weak var peerChannel: PTChannel?
    
    /** Prints out all errors and status updates */
    var debugMode = false
    
    
    
    // MARK: Methods
    
    /** Prints only if in debug mode */
    fileprivate func printDebug(_ string: String) {
        if debugMode {
            print(string)
        }
    }
    
    /** Begins to look for a device and connects when it finds one */
    func connect(portNumber: Int) {
        if !isConnected {
            self.portNumber = portNumber
            let channel = PTChannel(delegate: self)
            channel?.listen(onPort: in_port_t(portNumber), iPv4Address: INADDR_LOOPBACK, callback: { (error) in
                if error == nil {
                    self.serverChannel = channel
                }
            })
        }
    }
    
    /** Whether or not the device is connected */
    var isConnected: Bool {
        return peerChannel != nil
    }
    
    /** Closes the USB connectin */
    func disconnect() {
        self.serverChannel?.close()
        self.peerChannel?.close()
        peerChannel = nil
        serverChannel = nil
    }
    
    /** Sends data to the connected device
     * Uses NSKeyedArchiver to convert the object to data
     */
    func sendObject(object: Any, type: UInt32, completion: ((_ success: Bool) -> Void)? = nil) {
        let data = Data.toData(object: object)
        if peerChannel != nil {
            peerChannel?.sendFrame(ofType: type, tag: PTFrameNoTag, withPayload: (data as NSData).createReferencingDispatchData(), callback: { (error) in
                completion?(true)
            })
        } else {
            completion?(false)
        }
    }
    
    func sendDictionary(dictionary: NSDictionary, type: UInt32, completion: ((_ success: Bool) -> Void)? = nil) {
        if peerChannel != nil {
            peerChannel?.sendFrame(ofType: type, tag: PTFrameNoTag, withPayload: (dictionary as NSDictionary).createReferencingDispatchData(), callback:
                {(error) in
                completion?(true)
            })
        } else {
            completion?(false)
        }
    }
    
    /** Sends data to the connected device
     * Uses NSKeyedArchiver to convert the object to data
     */
    func sendPong(object: Any, type: UInt32, tagno: UInt32, completion: ((_ success: Bool) -> Void)? = nil) {
        let data = Data.toData(object: object)
        if peerChannel != nil {
            peerChannel?.sendFrame(ofType: type, tag: tagno, withPayload: (data as NSData).createReferencingDispatchData(), callback: { (error) in
                completion?(true)
            })
        } else {
            completion?(false)
        }
    }
    
    /** Sends data to the connected device */
    func sendData(data: Data, type: UInt32, completion: ((_ success: Bool) -> Void)? = nil) {
        if peerChannel != nil {
            peerChannel?.sendFrame(ofType: type, tag: PTFrameNoTag, withPayload: (data as NSData).createReferencingDispatchData(), callback: { (error) in
                completion?(true)
            })
        } else {
            completion?(false)
        }
    }
    
    /** Sends data to the connected device */
    func sendDispatchData(dispatchData: DispatchData, type: UInt32, completion: ((_ success: Bool) -> Void)? = nil) {
        if peerChannel != nil {
            peerChannel?.sendFrame(ofType: type, tag: PTFrameNoTag, withPayload: dispatchData as __DispatchData, callback: { (error) in
                completion?(true)
            })
        } else {
            completion?(false)
        }
    }
    
}

// MARK: - Channel Delegate
extension PTManager: PTChannelDelegate {
    
    func ioFrameChannel(_ channel: PTChannel!, shouldAcceptFrameOfType type: UInt32, tag: UInt32, payloadSize: UInt32) -> Bool {
        // Check if the channel is our connected channel; otherwise ignore it
        if channel != peerChannel {
            return false
        } else {
            return delegate!.peertalk(shouldAcceptDataOfType: type)
        }
    }
    
    func ioFrameChannel(_ channel: PTChannel!, didReceiveFrameOfType type: UInt32, tag: UInt32, payload: PTData!) {
        // Creates the data
        let dispatchData = payload.dispatchData as DispatchData
        let data = NSData(contentsOfDispatchData: dispatchData as __DispatchData) as Data
        delegate?.peertalk(didReceiveData: data, ofType: type, withTag: tag)
    }
    
    func ioFrameChannel(_ channel: PTChannel!, didEndWithError error: Error?) {
        printDebug("ERROR (Connection ended): \(String(describing: error?.localizedDescription))")
        peerChannel = nil
        serverChannel = nil
        delegate?.peertalk(didChangeConnection: false)
    }
    
    func ioFrameChannel(_ channel: PTChannel!, didAcceptConnection otherChannel: PTChannel!, from address: PTAddress!) {
        
        // Cancel any existing connections
        if (peerChannel != nil) {
            peerChannel?.cancel()
        }
        
        // Update the peer channel and information
        peerChannel = otherChannel
        peerChannel?.userInfo = address
        printDebug("SUCCESS (Connected to channel)")
        delegate?.peertalk(didChangeConnection: true)
    }
}

// MARK: - Data extension for conversion
extension Data {
    
    /** Unarchive data into an object. It will be returned as type `Any` but you can cast it into the correct type. */
    func convert() -> Any {
        return NSKeyedUnarchiver.unarchiveObject(with: self)!
    }
    
    /** Converts an object into Data using the NSKeyedArchiver */
    static func toData(object: Any) -> Data {
        return NSKeyedArchiver.archivedData(withRootObject: object)
    }
    
}
